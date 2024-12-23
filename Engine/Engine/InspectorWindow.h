#pragma once

#include "EditorWindow.h"

class InspectorWindow : public EditorWindow
{
public:
	InspectorWindow(const WindowType type, const std::string& name);
	~InspectorWindow();

	void DrawWindow() override;

private:
	char inputName[256] = "GameObject";
	ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
	bool isEditingInspector = false;
};