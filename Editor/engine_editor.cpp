#include "engine_editor.h"

#include "editor_scene.h"
#include "../Engine/scene_manager.h"

#include <DxLib.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

    return 0;
}

namespace editor
{
void Editor::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(GetMainWindowHandle());
    ImGui_ImplDX11_Init((ID3D11Device *)GetUseDirect3D11Device(),
                        (ID3D11DeviceContext *)GetUseDirect3D11DeviceContext());
}
void Editor::Update()
{}
void Editor::Draw()
{
    ImGui::ShowDemoWindow();
}
void Editor::Attach(engine::Engine engine)
{
    auto editor_scene = std::make_shared<EditorScene>();
    editor_scene->editor = this;
    engine::SceneManager::AddCustomScene(std::dynamic_pointer_cast<engine::Scene>(editor_scene));
}
}