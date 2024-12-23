#include "ComponentMaterial.h"
#include "GameObject.h"
#include "App.h"
#include "ModuleRenderer3D.h"

#include <windows.h>
#include <shellapi.h>
#include <algorithm>

ComponentMaterial::ComponentMaterial(GameObject* gameObject) : Component(gameObject, ComponentType::MATERIAL), materialTexture(nullptr), textureId(-1)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Update()
{
}

void ComponentMaterial::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (materialTexture->textureId != -1)
		{
			ImGui::Text("Path: %s", materialTexture->texturePath);
			ImGui::Text("Texture Size: %i x %i", materialTexture->textureWidth, materialTexture->textureHeight);
			ImGui::Image((ImTextureID)(uintptr_t)materialTexture->textureId, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::MenuItem("Show in Explorer"))
			{
				char buffer[MAX_PATH];
				GetModuleFileName(NULL, buffer, MAX_PATH);
				std::string::size_type pos = std::string(buffer).find_last_of("\\/");
				std::string exeDir = std::string(buffer).substr(0, pos);

				std::string path = exeDir + "\\..\\..\\Engine\\" + materialTexture->texturePath;

				std::replace(path.begin(), path.end(), '/', '\\');

				std::string command = "/select," + path;
				ShellExecute(NULL, "open", "explorer.exe", command.c_str(), NULL, SW_SHOWDEFAULT);
			}

			if (ImGui::MenuItem("Open Image"))
			{
				char buffer[MAX_PATH];
				GetModuleFileName(NULL, buffer, MAX_PATH);
				std::string::size_type pos = std::string(buffer).find_last_of("\\/");
				std::string exeDir = std::string(buffer).substr(0, pos);

				std::string path = exeDir + "\\..\\..\\Engine\\" + materialTexture->texturePath;

				ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
			}

			if (ImGui::Checkbox("Show Checkers Texture", &showCheckersTexture))
			{
				textureId = showCheckersTexture ? app->renderer3D->checkerTextureId : materialTexture->textureId;
			}
		}
	}
}

void ComponentMaterial::AddTexture(Texture* texture)
{
	if (gameObject->GetComponent(ComponentType::MESH))
	{
		if (!gameObject->GetComponent(ComponentType::MATERIAL))
		{
			gameObject->AddComponent(gameObject->material);
		}

		materialTexture = texture;
		textureId = materialTexture->textureId;
	}

	for (auto& child : gameObject->children)
	{
		if (!child->GetComponent(ComponentType::MATERIAL))
		{
			child->AddComponent(child->material);
		}

		child->material->materialTexture = texture;
		child->material->textureId = texture->textureId;
	}
}