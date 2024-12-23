#pragma once

#include "EditorWindow.h"

#include "glm/glm.hpp"

class PreferencesWindow : public EditorWindow
{
public:
	PreferencesWindow(const WindowType type, const std::string& name);
	~PreferencesWindow();

	void DrawWindow() override;

public:
	bool drawTextures = true;
	bool wireframe = false;
	bool shadedWireframe = false;
	bool cullFace = true;

	// Normals settings
	float vertexNormalLength = 0.1f;
	float faceNormalLength = 0.1f;
	glm::vec3 vertexNormalColor = glm::vec3(0, 1, 0);
	glm::vec3 faceNormalColor = glm::vec3(1, 0, 0);
};