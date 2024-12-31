#include "HierarchyWindow.h"
#include "App.h"

#include <algorithm>

HierarchyWindow::HierarchyWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

HierarchyWindow::~HierarchyWindow()
{
}

void HierarchyWindow::DeleteSelectedGameObject()
{
	GameObject* objectToDelete = app->editor->selectedGameObject;

	// Validaciones de seguridad
	if (!IsGameObjectValid(objectToDelete) || !IsGameObjectValid(objectToDelete->parent))
	{
		app->editor->selectedGameObject = nullptr;
		return;
	}

	if (objectToDelete == app->scene->root)
	{
		return; // No permitimos borrar el root
	}

	// Guardamos una referencia al padre antes de borrar
	GameObject* parent = objectToDelete->parent;

	// Limpiamos la selección antes de borrar
	app->editor->selectedGameObject = nullptr;

	// Encuentra y elimina el objeto de la lista de hijos del padre
	if (parent)
	{
		auto& parentChildren = parent->children;
		auto it = std::find(parentChildren.begin(), parentChildren.end(), objectToDelete);
		if (it != parentChildren.end())
		{
			parentChildren.erase(it);
		}
	}

	// Elimina el objeto y sus hijos recursivamente
	DeleteGameObjectRecursive(objectToDelete);
}

void HierarchyWindow::DeleteGameObjectRecursive(GameObject* gameObject)
{
	if (!IsGameObjectValid(gameObject))
	{
		return;
	}

	// Creamos una copia de la lista de hijos ya que la vamos a modificar
	std::vector<GameObject*> children = gameObject->children;

	// Limpiamos la lista de hijos original para evitar problemas de memoria
	gameObject->children.clear();

	// Eliminamos recursivamente todos los hijos
	for (auto* child : children)
	{
		if (IsGameObjectValid(child))
		{
			DeleteGameObjectRecursive(child);
		}
	}

	// Eliminamos los componentes
	for (auto* component : gameObject->components)
	{
		if (component)
		{
			delete component;
		}
	}
	gameObject->components.clear();

	// Limpiamos las referencias antes de eliminar
	gameObject->parent = nullptr;
	gameObject->transform = nullptr;  // Ya se eliminó en el bucle de componentes
	gameObject->mesh = nullptr;       // Ya se eliminó en el bucle de componentes
	gameObject->material = nullptr;   // Ya se eliminó en el bucle de componentes

	// Finalmente eliminamos el GameObject
	delete gameObject;
}

bool HierarchyWindow::IsGameObjectValid(GameObject* gameObject) const
{
	if (gameObject == nullptr)
		return false;

	// Verificación básica de dirección de memoria
	if (reinterpret_cast<uintptr_t>(gameObject) < 0x1000)
		return false;

	try
	{
		// Intenta acceder a algunas propiedades básicas para verificar que el objeto es válido
		volatile bool test = gameObject->isActive;
		volatile auto name = gameObject->name;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void HierarchyWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

	UpdateMouseState();

	// Añade la detección de la tecla Supr aquí
	if (app->editor->selectedGameObject != nullptr &&
		app->editor->selectedGameObject != app->scene->root &&
		ImGui::IsKeyPressed(ImGuiKey_Delete) &&
		ImGui::IsWindowFocused())
	{
		DeleteSelectedGameObject();
	}

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