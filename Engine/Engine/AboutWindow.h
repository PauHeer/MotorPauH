#pragma once

#include "EditorWindow.h"

class AboutWindow : public EditorWindow
{
public:
	AboutWindow(const WindowType type, const std::string& name);
	~AboutWindow();

	void DrawWindow() override;

	void HyperLink(const char* text, const char* url);
};