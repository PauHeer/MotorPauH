#pragma once
#include <string>
#include <glm/glm.hpp>
#include "Logger.h"

typedef unsigned int uint;

class Mesh
{
public:
    Mesh();
    ~Mesh();

    // Mesh data
    float* vertices;
    uint32_t* indices;
    float* normals;
    float* texCoords;

    uint verticesCount;
    uint indicesCount;
    uint normalsCount;
    uint texCoordsCount;

    uint verticesId;
    uint indicesId;
    uint normalsId;
    uint texCoordsId;

    // Material properties
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    glm::vec4 ambientColor;
    std::string diffuseTexturePath;

    // Public methods
    bool InitMesh();
    bool DrawMesh(uint textureId = 0, bool hasTexture = false, bool wireframe = false, bool cullface = true);
    bool DrawNormals(bool vertexNormals = true, bool faceNormals = false,
        float normalLength = 0.5f, float faceNormalLength = 0.5f,
        const glm::vec3& vertexNormalColor = glm::vec3(1, 1, 0),
        const glm::vec3& faceNormalColor = glm::vec3(1, 0, 0));
    void CleanUp();
    bool IsValid() const;

    // Setters for mesh data
    bool SetVertices(float* vertices, uint count);
    bool SetIndices(uint32_t* indices, uint count);
    bool SetNormals(float* normals, uint count);
    bool SetTexCoords(float* texCoords, uint count);

private:
    bool initialized;
    bool CheckMeshData() const;
    void ResetMesh();
};