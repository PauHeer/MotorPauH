#include "ComponentMesh.h"
#include "App.h"

#include <glm/gtc/type_ptr.hpp>

ComponentMesh::ComponentMesh(GameObject* gameObject) : Component(gameObject, ComponentType::MESH), mesh(nullptr)
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update()
{
    ComponentTransform* transform = gameObject->transform;

    if (transform != nullptr)
    {
        // Log para verificar los valores de transformación
        LOG(LogType::LOG_INFO, "Applying transform to mesh - Position: (%f, %f, %f), Scale: (%f, %f, %f)",
            transform->position.x, transform->position.y, transform->position.z,
            transform->scale.x, transform->scale.y, transform->scale.z);

        // Asegurarse de que la matriz está actualizada
        if (transform->updateTransform)
        {
            transform->UpdateTransform();
        }

        // Guardar la matriz actual
        glPushMatrix();

        // Aplicar la transformación global
        glMultMatrixf(glm::value_ptr(transform->globalTransform));
    }

    ComponentMaterial* material = gameObject->material;
    if (material != nullptr && mesh != nullptr)
    {
        mesh->DrawMesh(
            material->textureId,
            app->editor->preferencesWindow->drawTextures,
            app->editor->preferencesWindow->wireframe,
            app->editor->preferencesWindow->shadedWireframe
        );

        if (showVertexNormals || showFaceNormals)
        {
            mesh->DrawNormals(
                showVertexNormals,
                showFaceNormals,
                app->editor->preferencesWindow->vertexNormalLength,
                app->editor->preferencesWindow->faceNormalLength,
                app->editor->preferencesWindow->vertexNormalColor,
                app->editor->preferencesWindow->faceNormalColor
            );
        }
    }
    else
    {
        LOG(LogType::LOG_WARNING, "Mesh or Material is null!");
    }

    if (transform != nullptr)
    {
        glPopMatrix();
    }
}

void ComponentMesh::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Vertices: %d", mesh->verticesCount);
		ImGui::Text("Indices: %d", mesh->indicesCount);
		ImGui::Text("Normals: %d", mesh->normalsCount);
		ImGui::Text("Texture Coords: %d", mesh->texCoordsCount);

		ImGui::Spacing();

		ImGui::Checkbox("Vertex Normals", &showVertexNormals);
		ImGui::Checkbox("Face Normals", &showFaceNormals);
	}
}