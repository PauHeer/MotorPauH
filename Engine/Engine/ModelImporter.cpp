#include "ModelImporter.h"
#include "App.h"
#include "ComponentMesh.h"
#include <iostream>
#include <fstream>

bool ValidateMeshData(aiMesh* mesh, const char* meshName) {
    if (!mesh) {
        LOG(LogType::LOG_ERROR, "Null mesh pointer for %s", meshName);
        return false;
    }

    LOG(LogType::LOG_INFO, "Validating mesh: %s", meshName);
    LOG(LogType::LOG_INFO, " - Vertices: %d", mesh->mNumVertices);
    LOG(LogType::LOG_INFO, " - Faces: %d", mesh->mNumFaces);
    LOG(LogType::LOG_INFO, " - Has positions: %s", mesh->HasPositions() ? "yes" : "no");
    LOG(LogType::LOG_INFO, " - Has normals: %s", mesh->HasNormals() ? "yes" : "no");
    LOG(LogType::LOG_INFO, " - Has texture coords: %s", mesh->HasTextureCoords(0) ? "yes" : "no");

    if (!mesh->HasPositions()) {
        LOG(LogType::LOG_ERROR, "Mesh %s has no vertices", meshName);
        return false;
    }

    if (!mesh->HasNormals()) {
        LOG(LogType::LOG_WARNING, "Mesh %s has no normals", meshName);
    }

    if (!mesh->HasTextureCoords(0)) {
        LOG(LogType::LOG_WARNING, "Mesh %s has no texture coordinates", meshName);
    }

    return true;
}

bool ValidateScene(const aiScene* scene, const char* filePath) {
    if (!scene) {
        LOG(LogType::LOG_ERROR, "Null scene pointer for %s", filePath);
        return false;
    }

    LOG(LogType::LOG_INFO, "Validating scene from %s", filePath);
    LOG(LogType::LOG_INFO, " - Number of meshes: %d", scene->mNumMeshes);
    LOG(LogType::LOG_INFO, " - Number of materials: %d", scene->mNumMaterials);
    LOG(LogType::LOG_INFO, " - Number of textures: %d", scene->mNumTextures);

    if (!scene->mRootNode) {
        LOG(LogType::LOG_ERROR, "Scene has no root node");
        return false;
    }

    return true;
}

bool EnsureDirectoryExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            if (!std::filesystem::create_directories(path)) {
                LOG(LogType::LOG_ERROR, "Failed to create directory: %s", path.c_str());
                return false;
            }
            LOG(LogType::LOG_INFO, "Created directory: %s", path.c_str());
        }
        return true;
    }
    catch (const std::filesystem::filesystem_error& e) {
        LOG(LogType::LOG_ERROR, "Filesystem error: %s", e.what());
        return false;
    }
}

void CreateRequiredDirectories() {
    EnsureDirectoryExists("Assets");
    EnsureDirectoryExists("Assets/Models");
    EnsureDirectoryExists("Assets/Textures");
    EnsureDirectoryExists("Library");
    EnsureDirectoryExists("Library/Models");
    EnsureDirectoryExists("Library/Meshes");
}

ModelImporter::ModelImporter() {
    struct aiLogStream stream;
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
    aiAttachLogStream(&stream);
}

ModelImporter::~ModelImporter() {
    aiDetachAllLogStreams();
}
bool ModelImporter::SaveModel(Resource* resource) {
    if (!resource) {
        LOG(LogType::LOG_ERROR, "Invalid resource provided");
        return false;
    }

    const char* assetPath = resource->GetAssetFileDir().c_str();
    if (!assetPath || strlen(assetPath) == 0) {
        LOG(LogType::LOG_ERROR, "Invalid path provided");
        return false;
    }

    LOG(LogType::LOG_INFO, "Attempting to load model from: %s", assetPath);

    const aiScene* importedScene = aiImportFile(assetPath, aiProcessPreset_TargetRealtime_MaxQuality);
    if (!ValidateScene(importedScene, assetPath)) {
        LOG(LogType::LOG_ERROR, "Invalid scene: %s", aiGetErrorString());
        return false;
    }

    std::string fileName = assetPath;
    fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
    fileName = fileName.substr(0, fileName.find_last_of("."));

    SaveModelToCustomFile(importedScene, fileName);
    aiReleaseImport(importedScene);

    LOG(LogType::LOG_INFO, "%s model Saved", fileName.c_str());
    return true;
}

bool ModelImporter::LoadModel(Resource* resource, GameObject* root) {
    if (!root) {
        LOG(LogType::LOG_ERROR, "Invalid root GameObject provided");
        return false;
    }

    const char* path = resource->GetLibraryFileDir().c_str();
    std::string modelFilePath = path;

    LOG(LogType::LOG_INFO, "Attempting to load model from: %s", path);

    if (!std::filesystem::exists(modelFilePath)) {
        LOG(LogType::LOG_ERROR, "Model file does not exist: %s", path);
        return false;
    }

    std::ifstream modelFile(modelFilePath, std::ios::binary);
    if (!modelFile.good()) {
        LOG(LogType::LOG_ERROR, "Failed to open model file: %s", path);
        return false;
    }

    modelFile.close();
    LoadModelFromCustomFile(modelFilePath, root);
    LOG(LogType::LOG_INFO, "Model loaded successfully from: %s", path);
    return true;
}

void ModelImporter::SaveMeshToCustomFile(aiMesh* newMesh, const aiScene* scene, const std::string& filePath) {
    if (!EnsureDirectoryExists(std::filesystem::path(filePath).parent_path().string())) {
        LOG(LogType::LOG_ERROR, "Failed to create directory for mesh file");
        return;
    }

    if (!ValidateMeshData(newMesh, filePath.c_str())) {
        LOG(LogType::LOG_ERROR, "Invalid aiMesh data");
        return;
    }

    try {
        // Get counts
        const uint32_t ranges[4] = {
            newMesh->mNumFaces * 3,
            newMesh->mNumVertices,
            newMesh->mNumVertices,
            newMesh->mNumVertices
        };

        LOG(LogType::LOG_INFO, "Processing mesh data:");
        LOG(LogType::LOG_INFO, " - Indices: %d", ranges[0]);
        LOG(LogType::LOG_INFO, " - Vertices: %d", ranges[1]);
        LOG(LogType::LOG_INFO, " - Normals: %d", ranges[2]);
        LOG(LogType::LOG_INFO, " - TexCoords: %d", ranges[3]);

        // Process texture coordinates
        std::unique_ptr<float[]> texCoords(new float[static_cast<size_t>(newMesh->mNumVertices) * 2]);
        for (size_t i = 0; i < newMesh->mNumVertices; i++) {
            texCoords[i * 2] = newMesh->mTextureCoords[0][i].x;
            texCoords[i * 2 + 1] = newMesh->mTextureCoords[0][i].y;
        }

        // Process indices
        std::unique_ptr<uint32_t[]> indices(new uint32_t[static_cast<size_t>(newMesh->mNumFaces) * 3]);
        for (size_t i = 0; i < static_cast<size_t>(newMesh->mNumFaces); ++i) {
            if (newMesh->mFaces[i].mNumIndices != 3) {
                LOG(LogType::LOG_WARNING, "Face %zu does not have 3 indices", i);
                continue;
            }
            memcpy(&indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint32_t));
        }

        // Material properties
        glm::vec4 diffuseColor(1.0f);
        glm::vec4 specularColor(1.0f);
        glm::vec4 ambientColor(1.0f);
        std::string diffuseTexturePath;

        // Process material
        if (newMesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[newMesh->mMaterialIndex];
            aiColor4D color;

            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) {
                diffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
                LOG(LogType::LOG_INFO, "Loaded diffuse color");
            }

            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)) {
                specularColor = glm::vec4(color.r, color.g, color.b, color.a);
                LOG(LogType::LOG_INFO, "Loaded specular color");
            }

            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color)) {
                ambientColor = glm::vec4(color.r, color.g, color.b, color.a);
                LOG(LogType::LOG_INFO, "Loaded ambient color");
            }

            aiString texturePath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
                std::string basePath = "Assets/Textures/";
                if (app->fileSystem->FileExists(basePath + texturePath.C_Str())) {
                    diffuseTexturePath = basePath + texturePath.C_Str();
                    app->importer->ImportFile(diffuseTexturePath.c_str(), false);
                }
            }
        }
        // Calculate total size needed
        const size_t size = sizeof(ranges)
            + (sizeof(uint32_t) * ranges[0])
            + (sizeof(float) * ranges[1] * 3)
            + (sizeof(float) * ranges[2] * 3)
            + (sizeof(float) * ranges[3] * 2)
            + (sizeof(glm::vec4) * 3)
            + sizeof(uint32_t)
            + diffuseTexturePath.size() + 1;

        // Write to file
        std::ofstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create mesh file");
        }

        // Write all data
        file.write(reinterpret_cast<const char*>(ranges), sizeof(ranges));
        file.write(reinterpret_cast<const char*>(indices.get()), sizeof(uint32_t) * ranges[0]);
        file.write(reinterpret_cast<const char*>(newMesh->mVertices), sizeof(float) * ranges[1] * 3);
        file.write(reinterpret_cast<const char*>(newMesh->mNormals), sizeof(float) * ranges[2] * 3);
        file.write(reinterpret_cast<const char*>(texCoords.get()), sizeof(float) * ranges[3] * 2);
        file.write(reinterpret_cast<const char*>(&diffuseColor), sizeof(glm::vec4));
        file.write(reinterpret_cast<const char*>(&specularColor), sizeof(glm::vec4));
        file.write(reinterpret_cast<const char*>(&ambientColor), sizeof(glm::vec4));

        uint32_t texturePathLength = static_cast<uint32_t>(diffuseTexturePath.size());
        file.write(reinterpret_cast<const char*>(&texturePathLength), sizeof(uint32_t));
        file.write(diffuseTexturePath.c_str(), texturePathLength + 1);

        file.close();
        LOG(LogType::LOG_INFO, "Mesh saved successfully to: %s", filePath.c_str());
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Failed to save mesh: %s", e.what());
    }
}

Mesh* ModelImporter::LoadMeshFromCustomFile(const std::string& filePath)
{
    LOG(LogType::LOG_INFO, "Attempting to load mesh from: %s", filePath.c_str());

    if (!std::filesystem::exists(filePath)) {
        LOG(LogType::LOG_ERROR, "Mesh file does not exist: %s", filePath.c_str());
        return nullptr;
    }

    std::ifstream meshFile(filePath, std::ios::binary);
    if (!meshFile.is_open()) {
        LOG(LogType::LOG_ERROR, "Failed to open file for loading data: %s", filePath.c_str());
        return nullptr;
    }

    try {
        // Crear un nuevo mesh
        Mesh* mesh = new Mesh();
        if (!mesh) {
            throw std::runtime_error("Failed to allocate memory for mesh");
        }

        // Leer rangos
        uint32_t ranges[4] = { 0, 0, 0, 0 };
        if (!meshFile.read(reinterpret_cast<char*>(ranges), sizeof(ranges))) {
            throw std::runtime_error("Failed to read ranges");
        }

        LOG(LogType::LOG_INFO, "Mesh data ranges:");
        LOG(LogType::LOG_INFO, " - Indices: %d", ranges[0]);
        LOG(LogType::LOG_INFO, " - Vertices: %d", ranges[1]);
        LOG(LogType::LOG_INFO, " - Normals: %d", ranges[2]);
        LOG(LogType::LOG_INFO, " - TexCoords: %d", ranges[3]);

        // Verificar rangos
        if (ranges[0] == 0 || ranges[1] == 0) {
            throw std::runtime_error("Invalid ranges in file (zero vertices or indices)");
        }

        // Asignar rangos
        mesh->indicesCount = ranges[0];
        mesh->verticesCount = ranges[1];
        mesh->normalsCount = ranges[2];
        mesh->texCoordsCount = ranges[3];

        // Asignar memoria
        try {
            mesh->indices = new uint32_t[mesh->indicesCount];
            mesh->vertices = new float[mesh->verticesCount * 3];
            mesh->normals = new float[mesh->normalsCount * 3];
            mesh->texCoords = new float[mesh->texCoordsCount * 2];
        }
        catch (const std::bad_alloc& e) {
            throw std::runtime_error("Failed to allocate memory for mesh data: " + std::string(e.what()));
        }

        // Leer datos
        if (!meshFile.read(reinterpret_cast<char*>(mesh->indices), sizeof(uint32_t) * mesh->indicesCount) ||
            !meshFile.read(reinterpret_cast<char*>(mesh->vertices), sizeof(float) * mesh->verticesCount * 3) ||
            !meshFile.read(reinterpret_cast<char*>(mesh->normals), sizeof(float) * mesh->normalsCount * 3) ||
            !meshFile.read(reinterpret_cast<char*>(mesh->texCoords), sizeof(float) * mesh->texCoordsCount * 2)) {
            throw std::runtime_error("Failed to read mesh data");
        }

        // Validar índices
        for (uint32_t i = 0; i < mesh->indicesCount; i++) {
            if (mesh->indices[i] >= mesh->verticesCount) {
                throw std::runtime_error("Invalid index found: index out of bounds");
            }
        }

        // Leer colores del material
        if (!meshFile.read(reinterpret_cast<char*>(&mesh->diffuseColor), sizeof(glm::vec4)) ||
            !meshFile.read(reinterpret_cast<char*>(&mesh->specularColor), sizeof(glm::vec4)) ||
            !meshFile.read(reinterpret_cast<char*>(&mesh->ambientColor), sizeof(glm::vec4))) {
            throw std::runtime_error("Failed to read material colors");
        }

        // Leer path de la textura
        uint32_t texturePathLength = 0;
        if (!meshFile.read(reinterpret_cast<char*>(&texturePathLength), sizeof(uint32_t))) {
            throw std::runtime_error("Failed to read texture path length");
        }

        if (texturePathLength > 0) {
            std::vector<char> tempBuffer(texturePathLength + 1, 0);
            if (!meshFile.read(tempBuffer.data(), texturePathLength)) {
                throw std::runtime_error("Failed to read texture path");
            }
            mesh->diffuseTexturePath = std::string(tempBuffer.data(), texturePathLength);
            LOG(LogType::LOG_INFO, "Loaded texture path: %s", mesh->diffuseTexturePath.c_str());
        }

        meshFile.close();

        // Inicializar mesh
        if (!mesh->InitMesh()) {
            throw std::runtime_error("Failed to initialize mesh");
        }

        LOG(LogType::LOG_INFO, "Mesh loaded successfully: %s", filePath.c_str());
        return mesh;
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Exception while loading mesh %s: %s", filePath.c_str(), e.what());
        meshFile.close();
        return nullptr;
    }
}

void ModelImporter::SaveModelToCustomFile(const aiScene* scene, const std::string& fileName) {
    if (!ValidateScene(scene, fileName.c_str())) {
        LOG(LogType::LOG_ERROR, "Invalid scene for model: %s", fileName.c_str());
        return;
    }

    try {
        // Save meshes to custom files
        std::vector<std::string> meshPaths;
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            std::string meshPath = "Library/Meshes/" + fileName + std::to_string(i) + ".mesh";
            SaveMeshToCustomFile(scene->mMeshes[i], scene, meshPath);
            meshPaths.push_back(meshPath);
            LOG(LogType::LOG_INFO, "Saved mesh %d/%d", i + 1, scene->mNumMeshes);
        }

        // Prepare buffer for model file
        std::vector<char> buffer;
        size_t currentPos = 0;

        // Save number of meshes
        uint32_t numMeshes = scene->mNumMeshes;
        buffer.resize(sizeof(uint32_t));
        memcpy(buffer.data(), &numMeshes, sizeof(uint32_t));
        currentPos = sizeof(uint32_t);

        // Save mesh paths
        for (const auto& path : meshPaths) {
            uint32_t pathLength = static_cast<uint32_t>(path.size());

            // Resize buffer for new data
            buffer.resize(buffer.size() + sizeof(uint32_t) + pathLength + 1);

            // Save path length and path
            memcpy(buffer.data() + currentPos, &pathLength, sizeof(uint32_t));
            currentPos += sizeof(uint32_t);
            memcpy(buffer.data() + currentPos, path.c_str(), pathLength + 1);
            currentPos += pathLength + 1;
        }

        // Calculate and reserve space for node hierarchy
        size_t nodeSize = CalculateNodeSize(scene->mRootNode);
        buffer.resize(buffer.size() + nodeSize);

        // Save node hierarchy
        SaveNodeToBuffer(scene->mRootNode, buffer, currentPos);

        // Write to file
        std::string modelPath = "Library/Models/" + fileName + ".model";
        std::ofstream file(modelPath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create model file: " + modelPath);
        }

        file.write(buffer.data(), buffer.size());
        file.close();
        LOG(LogType::LOG_INFO, "Model saved successfully to: %s", modelPath.c_str());
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Failed to save model: %s", e.what());
    }
}

void ModelImporter::LoadModelFromCustomFile(const std::string& filePath, GameObject* root) {
    try {
        LOG(LogType::LOG_INFO, "Loading model from: %s", filePath.c_str());

        if (!root) {
            throw std::runtime_error("Invalid root GameObject");
        }

        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open model file");
        }

        // Leer el archivo en un buffer
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if (fileSize == 0) {
            throw std::runtime_error("Model file is empty");
        }

        std::vector<char> buffer(fileSize);
        if (!file.read(buffer.data(), fileSize)) {
            throw std::runtime_error("Failed to read model file");
        }
        file.close();

        size_t currentPos = 0;

        // Leer número de meshes
        uint32_t numMeshes = 0;
        if (currentPos + sizeof(uint32_t) > buffer.size()) {
            throw std::runtime_error("Invalid file format: can't read number of meshes");
        }
        memcpy(&numMeshes, buffer.data() + currentPos, sizeof(uint32_t));
        currentPos += sizeof(uint32_t);

        LOG(LogType::LOG_INFO, "Model contains %d meshes", numMeshes);

        // Cargar meshes
        std::vector<Mesh*> meshes;
        for (uint32_t i = 0; i < numMeshes && currentPos < buffer.size(); i++) {
            // Leer longitud del path
            uint32_t pathLength = 0;
            if (currentPos + sizeof(uint32_t) > buffer.size()) {
                break;
            }

            memcpy(&pathLength, buffer.data() + currentPos, sizeof(uint32_t));
            currentPos += sizeof(uint32_t);

            if (currentPos + pathLength + 1 > buffer.size()) {
                break;
            }

            std::string meshPath(buffer.data() + currentPos, pathLength);
            currentPos += pathLength + 1;

            LOG(LogType::LOG_INFO, "Loading mesh %d/%d: %s", i + 1, numMeshes, meshPath.c_str());

            // Cargar mesh desde archivo
            Mesh* mesh = LoadMeshFromCustomFile(meshPath);
            if (mesh && mesh->IsValid()) {
                meshes.push_back(mesh);
                LOG(LogType::LOG_INFO, "Successfully loaded mesh %d/%d", i + 1, numMeshes);
            }
            else {
                LOG(LogType::LOG_ERROR, "Failed to load mesh %d/%d", i + 1, numMeshes);
                delete mesh;
            }
        }

        if (meshes.empty()) {
            throw std::runtime_error("No meshes were loaded successfully");
        }

        // Obtener nombre del archivo
        std::string fileName = filePath;
        fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
        fileName = fileName.substr(0, fileName.find_last_of("."));

        // Cargar jerarquía de nodos
        LoadNodeFromBuffer(buffer.data(), currentPos, meshes, root, fileName.c_str());

        LOG(LogType::LOG_INFO, "Model loaded successfully");
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Exception in LoadModelFromCustomFile: %s", e.what());
    }
}

void ModelImporter::LoadNodeFromBuffer(const char* buffer, size_t& currentPos,
    std::vector<Mesh*>& meshes, GameObject* parent,
    const char* fileName) {
    try {
        // Read node name
        uint32_t nameLength;
        memcpy(&nameLength, buffer + currentPos, sizeof(uint32_t));
        currentPos += sizeof(uint32_t);

        std::string nodeName(buffer + currentPos, nameLength);
        currentPos += nameLength + 1;

        // Read number of meshes
        uint32_t numMeshes;
        memcpy(&numMeshes, buffer + currentPos, sizeof(uint32_t));
        currentPos += sizeof(uint32_t);

        // Create GameObject if there are meshes
        GameObject* gameObjectNode = nullptr;
        if (numMeshes > 0) {
            gameObjectNode = new GameObject(nodeName.c_str(), parent);

            // Process meshes
            for (uint32_t i = 0; i < numMeshes; i++) {
                uint32_t meshIndex;
                memcpy(&meshIndex, buffer + currentPos, sizeof(uint32_t));
                currentPos += sizeof(uint32_t);

                if (meshIndex < meshes.size() && meshes[meshIndex]) {
                    ComponentMesh* componentMesh = dynamic_cast<ComponentMesh*>(
                        gameObjectNode->AddComponent(gameObjectNode->mesh));

                    if (componentMesh) {
                        componentMesh->mesh = meshes[meshIndex];

                        if (!meshes[meshIndex]->diffuseTexturePath.empty()) {
                            std::string extension = app->fileSystem->GetExtension(
                                meshes[meshIndex]->diffuseTexturePath);
                            ResourceType resourceType = app->resources->GetResourceTypeFromExtension(extension);
                            Resource* newResource = app->resources->FindResourceInLibrary(
                                meshes[meshIndex]->diffuseTexturePath, resourceType);

                            if (newResource) {
                                Texture* newTexture = app->importer->textureImporter->LoadTextureImage(newResource);
                                if (newTexture) {
                                    gameObjectNode->material->AddTexture(newTexture);
                                }
                            }
                        }
                    }
                }
            }

            parent->children.push_back(gameObjectNode);
        }

        // Read number of children
        uint32_t numChildren;
        memcpy(&numChildren, buffer + currentPos, sizeof(uint32_t));
        currentPos += sizeof(uint32_t);

        // Process children nodes
        if (numChildren > 0) {
            GameObject* holder = gameObjectNode ? gameObjectNode : new GameObject(fileName, parent);
            if (!gameObjectNode) {
                parent->children.push_back(holder);
            }

            for (uint32_t i = 0; i < numChildren; i++) {
                LoadNodeFromBuffer(buffer, currentPos, meshes, holder, nodeName.c_str());
            }
        }
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Error processing node: %s", e.what());
    }
}

void ModelImporter::SaveNodeToBuffer(const aiNode* node, std::vector<char>& buffer, size_t& currentPos) {
    if (!node) return;

    // Save node name
    uint32_t nameLength = static_cast<uint32_t>(strlen(node->mName.C_Str()));
    memcpy(buffer.data() + currentPos, &nameLength, sizeof(uint32_t));
    currentPos += sizeof(uint32_t);
    memcpy(buffer.data() + currentPos, node->mName.C_Str(), nameLength + 1);
    currentPos += nameLength + 1;

    // Save meshes
    memcpy(buffer.data() + currentPos, &node->mNumMeshes, sizeof(uint32_t));
    currentPos += sizeof(uint32_t);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        memcpy(buffer.data() + currentPos, &node->mMeshes[i], sizeof(uint32_t));
        currentPos += sizeof(uint32_t);
    }

    // Save children
    memcpy(buffer.data() + currentPos, &node->mNumChildren, sizeof(uint32_t));
    currentPos += sizeof(uint32_t);

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        SaveNodeToBuffer(node->mChildren[i], buffer, currentPos);
    }
}

size_t ModelImporter::CalculateNodeSize(const aiNode* node) {
    if (!node) return 0;

    size_t size = 0;

    // Node name size
    size += sizeof(uint32_t) + strlen(node->mName.C_Str()) + 1;

    // Meshes size
    size += sizeof(uint32_t);
    size += sizeof(uint32_t) * node->mNumMeshes;

    // Children count size
    size += sizeof(uint32_t);

    // Children nodes size
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        size += CalculateNodeSize(node->mChildren[i]);
    }

    return size;
}