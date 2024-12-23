#include "HierarchyWindow.h"
#include "App.h"

#include <algorithm>

HierarchyWindow::HierarchyWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

HierarchyWindow::~HierarchyWindow()
{
}

void HierarchyWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

	UpdateMouseState();

	if (ImGui::Button("+", ImVec2(20, 20)))
	{
		ImGui::OpenPopup("GameObject");
	}

	if (ImGui::BeginPopup("GameObject"))
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			app->scene->CreateGameObject("GameObject", app->scene->root);
			app->editor->selectedGameObject = app->scene->root->children.back();
		}
		if (ImGui::BeginMenu("3D Object"))
		{
			const char* objectNames[] = { "Cube", "Sphere", "Capsule", "Cylinder" };
			const char* basePath = "Engine/Primitives/";
			const char* extension = ".fbx";

			for (const char* name : objectNames)
			{
				std::string fullPath = std::string(basePath) + name + extension;

				if (ImGui::MenuItem(name))
				{
					Resource* resource = app->resources->FindResourceInLibrary(fullPath, ResourceType::MODEL);
					if (!resource)
						resource = app->importer->ImportFileToLibrary(fullPath, ResourceType::MODEL);

					app->importer->modelImporter->LoadModel(resource, app->scene->root);
					app->editor->selectedGameObject = app->scene->root->children.back();
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##Search", "Search", searchInput, 256);

	ImGui::BeginGroup();

	HierarchyTree(app->scene->root, true, searchInput);

	ImVec2 availableSize = ImGui::GetContentRegionAvail();

	ImGui::Dummy(availableSize);

	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
		{
			const char* droppedFilePath = static_cast<const char*>(payload->Data);
			app->importer->ImportFile(droppedFilePath, true);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

void HierarchyWindow::HierarchyTree(GameObject* node, bool isRoot, const char* searchText)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (isRoot)
	{
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	if (node->children.empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool isSelected = (app->editor->selectedGameObject == node);

	if (isSelected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (!node->isActive)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	}

	if (FilterNode(node, searchText))
	{
		bool isOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());

		if (ImGui::IsItemClicked())
		{
			if (app->editor->selectedGameObject && app->editor->selectedGameObject->isEditing)
			{
				app->editor->selectedGameObject->isEditing = false;
			}
			app->editor->selectedGameObject = node;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && !ImGui::IsItemToggledOpen())
		{
			node->isEditing = true;
		}

		// Rename node
		if (node->isEditing)
		{
			strcpy_s(inputName, app->editor->selectedGameObject->name.c_str());
			ImGui::SetNextItemWidth(ImGui::CalcTextSize(node->name.c_str()).x + 100);
			if (ImGui::InputText("##edit", inputName, sizeof(inputName), inputTextFlags)
				|| (!ImGui::IsItemActive() && !ImGui::IsAnyItemActive()))
			{
				if (inputName[0] != '\0') node->name = inputName;
				node->isEditing = false;
			}

			ImGui::SetKeyboardFocusHere(-1);
		}

		// Create child nodes
		if (isOpen && !node->children.empty())
		{
			for (unsigned int i = 0; i < node->children.size(); i++)
			{
				HierarchyTree(node->children[i], false, searchText);
			}
			ImGui::TreePop();
		}

		if (!node->isActive)
		{
			ImGui::PopStyleColor();
		}
	}
	else
	{
		for (unsigned int i = 0; i < node->children.size(); i++)
		{
			HierarchyTree(node->children[i], false, searchText);
		}
	}
}

bool HierarchyWindow::FilterNode(GameObject* node, const char* searchText)
{
	std::string nodeNameLower = node->name;
	std::transform(nodeNameLower.begin(), nodeNameLower.end(), nodeNameLower.begin(), ::tolower);

	std::string searchTextLower = searchText;
	std::transform(searchTextLower.begin(), searchTextLower.end(), searchTextLower.begin(), ::tolower);

	return nodeNameLower.find(searchTextLower) != std::string::npos;
}