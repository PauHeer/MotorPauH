#include "AboutWindow.h"

#include <windows.h>
#include <shellapi.h>

AboutWindow::AboutWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

	UpdateMouseState();

    // Title
    ImGui::Text("SpectraEngine");
    ImGui::Separator();

    // Description
    ImGui::Text("Description:");
    ImGui::TextWrapped("SpectraEngine is a 3D game engine developed in C++ with OpenGL, currently capable of rendering geometry with a range of controls and information displays.");
    ImGui::Spacing();
    HyperLink("GitHub", "https://github.com/CITM-UPC/SpectraEngine");
    ImGui::Spacing();

    ImGui::Separator();

    // Team Members
    ImGui::Text("Team Members:");
	ImGui::Spacing();
    HyperLink("Guillem Alqueza", "https://github.com/guillemalqueza");
    HyperLink("Miguel Iglesias", "https://github.com/MiguelIglesiasAbarca");
    HyperLink("Lluc Cid", "https://github.com/Lluccib");
    ImGui::Spacing();

    ImGui::Separator();

    // Viewer Controls
    ImGui::Text("Viewer Controls:");
    ImGui::Spacing();
    ImGui::BulletText("Right-click: Move the current view.");
    ImGui::BulletText("Right-click + WASD: Fly around the scene.");
    ImGui::BulletText("Alt + Right-click or Mouse Wheel: Zoom in or out.");
    ImGui::BulletText("Alt + Left-click: Orbit the camera around a pivot point.");
    ImGui::BulletText("Alt + Mouse Wheel: Pan the camera.");
    ImGui::BulletText("F: Frame the selected object.");
    ImGui::Spacing();

    ImGui::Separator();

    // Libraries and Dependencies
    ImGui::Text("Libraries and Dependencies:");
    ImGui::Spacing();
    HyperLink("SDL2", "https://github.com/libsdl-org/SDL");
    HyperLink("OpenGL", "https://www.opengl.org/");
    HyperLink("ImGui", "https://github.com/ocornut/imgui");
    HyperLink("GLM", "https://github.com/g-truc/glm");
    HyperLink("Assimp", "https://github.com/assimp/assimp");
    HyperLink("DeviL", "https://github.com/DentonW/DevIL");

    ImGui::End();
}

void AboutWindow::HyperLink(const char* text, const char* url)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
    ImGui::Text(text);
    if (ImGui::IsItemHovered()) 
    {
        ImGui::SetTooltip(url);
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    if (ImGui::IsItemClicked()) 
    {
        ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    }
    ImGui::PopStyleColor();
}