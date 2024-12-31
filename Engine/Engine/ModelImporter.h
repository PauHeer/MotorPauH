#pragma once

#include "Mesh.h"
#include "GameObject.h"
#include "Resource.h"

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>

class ModelImporter
{
public:
    ModelImporter();
    ~ModelImporter();

    // Main public interface
    bool SaveModel(Resource* resource);
    bool LoadModel(Resource* resource, GameObject* root);

private:
    // Model saving functions
    void SaveModelToCustomFile(const aiScene* scene, const std::string& fileName);
    void SaveNodeToBuffer(const aiNode* node, std::vector<char>& buffer, size_t& currentPos);
    void SaveMeshToCustomFile(aiMesh* mesh, const aiScene* scene, const std::string& filePath);

    // Model loading functions
    void LoadModelFromCustomFile(const std::string& filePath, GameObject* root);
    void LoadNodeFromBuffer(const char* buffer, size_t& currentPos,
        std::vector<Mesh*>& meshes, GameObject* parent,
        const char* fileName);
    Mesh* LoadMeshFromCustomFile(const std::string& filePath);

    // Utility functions
    size_t CalculateNodeSize(const aiNode* node);
};