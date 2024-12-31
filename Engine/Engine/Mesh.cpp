#include "Mesh.h"
#include "GL/glew.h"
#include "Logger.h"

Mesh::Mesh() :
    vertices(nullptr),
    indices(nullptr),
    normals(nullptr),
    texCoords(nullptr),
    verticesCount(0),
    indicesCount(0),
    normalsCount(0),
    texCoordsCount(0),
    verticesId(0),
    indicesId(0),
    normalsId(0),
    texCoordsId(0),
    initialized(false)
{
    diffuseColor = glm::vec4(1.0f);
    specularColor = glm::vec4(1.0f);
    ambientColor = glm::vec4(1.0f);
    LOG(LogType::LOG_INFO, "Mesh created");
}

Mesh::~Mesh()
{
    CleanUp();
}

bool Mesh::SetVertices(float* newVertices, uint count)
{
    if (newVertices == nullptr || count == 0) {
        LOG(LogType::LOG_ERROR, "Invalid vertices data");
        return false;
    }

    if (vertices != nullptr) {
        delete[] vertices;
    }

    vertices = new float[count * 3];
    memcpy(vertices, newVertices, count * 3 * sizeof(float));
    verticesCount = count;
    return true;
}

bool Mesh::SetIndices(uint32_t* newIndices, uint count)
{
    if (newIndices == nullptr || count == 0) {
        LOG(LogType::LOG_ERROR, "Invalid indices data");
        return false;
    }

    if (indices != nullptr) {
        delete[] indices;
    }

    indices = new uint32_t[count];
    memcpy(indices, newIndices, count * sizeof(uint32_t));
    indicesCount = count;
    return true;
}

bool Mesh::SetNormals(float* newNormals, uint count)
{
    if (newNormals == nullptr || count == 0) {
        LOG(LogType::LOG_ERROR, "Invalid normals data");
        return false;
    }

    if (normals != nullptr) {
        delete[] normals;
    }

    normals = new float[count * 3];
    memcpy(normals, newNormals, count * 3 * sizeof(float));
    normalsCount = count;
    return true;
}

bool Mesh::SetTexCoords(float* newTexCoords, uint count)
{
    if (newTexCoords == nullptr || count == 0) {
        LOG(LogType::LOG_ERROR, "Invalid texture coordinates data");
        return false;
    }

    if (texCoords != nullptr) {
        delete[] texCoords;
    }

    texCoords = new float[count * 2];
    memcpy(texCoords, newTexCoords, count * 2 * sizeof(float));
    texCoordsCount = count;
    return true;
}

bool Mesh::InitMesh()
{
    if (!CheckMeshData()) {
        LOG(LogType::LOG_ERROR, "Failed to initialize mesh: Invalid mesh data");
        return false;
    }

    if (initialized) {
        CleanUp();
    }

    try {
        // Vertices
        glGenBuffers(1, &verticesId);
        glBindBuffer(GL_ARRAY_BUFFER, verticesId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesCount * 3, vertices, GL_STATIC_DRAW);

        // Indices
        glGenBuffers(1, &indicesId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indicesCount, indices, GL_STATIC_DRAW);

        // Normals
        glGenBuffers(1, &normalsId);
        glBindBuffer(GL_ARRAY_BUFFER, normalsId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalsCount * 3, normals, GL_STATIC_DRAW);

        // Texture Coords
        glGenBuffers(1, &texCoordsId);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoordsCount * 2, texCoords, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        initialized = true;
        LOG(LogType::LOG_INFO, "Mesh initialized successfully");
        return true;
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Error initializing mesh: %s", e.what());
        CleanUp();
        return false;
    }
}

bool Mesh::DrawMesh(uint textureId, bool hasTexture, bool wireframe, bool cullface)
{
    if (!initialized || !CheckMeshData()) {
        LOG(LogType::LOG_ERROR, "Cannot draw mesh: Mesh not initialized or invalid data");
        return false;
    }

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (cullface)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if (hasTexture && textureId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    glBindBuffer(GL_ARRAY_BUFFER, verticesId);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, normalsId);
    glNormalPointer(GL_FLOAT, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, NULL);

    if (hasTexture && textureId != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return true;
}

bool Mesh::DrawNormals(bool vertexNormals, bool faceNormals, float normalLength,
    float faceNormalLength, const glm::vec3& vertexNormalColor,
    const glm::vec3& faceNormalColor)
{
    if (!initialized || !CheckMeshData()) {
        LOG(LogType::LOG_ERROR, "Cannot draw normals: Mesh not initialized or invalid data");
        return false;
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Draw vertex normals
    if (vertexNormals)
    {
        glBegin(GL_LINES);
        glColor3f(vertexNormalColor.x, vertexNormalColor.y, vertexNormalColor.z);

        for (uint i = 0; i < verticesCount; i++)
        {
            glVertex3f(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
            glVertex3f(
                vertices[i * 3] + normals[i * 3] * normalLength,
                vertices[i * 3 + 1] + normals[i * 3 + 1] * normalLength,
                vertices[i * 3 + 2] + normals[i * 3 + 2] * normalLength
            );
        }
        glEnd();
    }

    // Draw face normals
    if (faceNormals)
    {
        glBegin(GL_LINES);
        glColor3f(faceNormalColor.x, faceNormalColor.y, faceNormalColor.z);

        for (uint i = 0; i < indicesCount; i += 3)
        {
            // Calculate face center and normal
            glm::vec3 v1(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
            glm::vec3 v2(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
            glm::vec3 v3(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);

            glm::vec3 center = (v1 + v2 + v3) / 3.0f;

            glm::vec3 edge1 = v2 - v1;
            glm::vec3 edge2 = v3 - v1;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

            glVertex3f(center.x, center.y, center.z);
            glVertex3f(
                center.x + normal.x * faceNormalLength,
                center.y + normal.y * faceNormalLength,
                center.z + normal.z * faceNormalLength
            );
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
    return true;
}

void Mesh::CleanUp()
{
    if (verticesId != 0) {
        glDeleteBuffers(1, &verticesId);
        verticesId = 0;
    }
    if (indicesId != 0) {
        glDeleteBuffers(1, &indicesId);
        indicesId = 0;
    }
    if (normalsId != 0) {
        glDeleteBuffers(1, &normalsId);
        normalsId = 0;
    }
    if (texCoordsId != 0) {
        glDeleteBuffers(1, &texCoordsId);
        texCoordsId = 0;
    }

    delete[] vertices;
    delete[] indices;
    delete[] normals;
    delete[] texCoords;

    vertices = nullptr;
    indices = nullptr;
    normals = nullptr;
    texCoords = nullptr;

    verticesCount = 0;
    indicesCount = 0;
    normalsCount = 0;
    texCoordsCount = 0;

    initialized = false;
    LOG(LogType::LOG_INFO, "Mesh cleaned up successfully");
}

bool Mesh::IsValid() const
{
    return initialized && CheckMeshData();
}

bool Mesh::CheckMeshData() const
{
    return vertices != nullptr && indices != nullptr &&
        normals != nullptr && texCoords != nullptr &&
        verticesCount > 0 && indicesCount > 0 &&
        normalsCount > 0 && texCoordsCount > 0;
}