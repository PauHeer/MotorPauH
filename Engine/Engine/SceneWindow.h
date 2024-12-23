#pragma once

#include "EditorWindow.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow(const WindowType type, const std::string& name);
	~SceneWindow();

	void DrawWindow() override;
};