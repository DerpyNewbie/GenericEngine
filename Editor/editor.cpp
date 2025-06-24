#include "pch.h"

#include "editor.h"
#include "editor_prefs.h"
#include "hierarchy.h"
#include "inspector.h"
#include "profiler.h"
#include "update_man_debugger.h"

#include "application.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "DxLib/dxlib_helper.h"
#include "update_manager.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

#include <ranges>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SetUseHookWinProcReturnValue(true);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace editor
{
void Editor::SetEditorStyle(const int i)
{
    if (m_last_editor_style_ == i)
        return;
    switch (i)
    {
    default:
    case 0:
        ImGui::StyleColorsDark();
        break;
    case 1:
        ImGui::StyleColorsLight();
        break;
    case 2:
        ImGui::StyleColorsClassic();
        break;
    }

    m_last_editor_style_ = i;
}

void Editor::Init()
{
    {
        Application::AddWindowCallback(WndProc);
        // imgui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        SetEditorStyle(0);

        ImFontConfig font_config;
        font_config.MergeMode = true;
        font_config.GlyphRanges = io.Fonts->GetGlyphRangesJapanese();
        io.Fonts->AddFontDefault();
        ImFont *font = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\meiryo.ttc)", 18.0f, &font_config,
                                                    io.Fonts->GetGlyphRangesJapanese());
        IM_ASSERT(font != nullptr);

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heap_desc.NumDescriptors = 1;
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heap_desc.NodeMask = 0;

        g_RenderEngine->Device()->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_pD3DSrvDescHeap));

        // フォントディスクリプタ取得
        D3D12_CPU_DESCRIPTOR_HANDLE font_cpu_desc_handle = m_pD3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE font_gpu_desc_handle = m_pD3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart();

        ImGui_ImplWin32_Init(Application::GetWindowHandle());
        ImGui_ImplDX12_Init(
            g_RenderEngine->Device(),
            RenderEngine::FRAME_BUFFER_COUNT,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            m_pD3DSrvDescHeap,
            font_cpu_desc_handle,
            font_gpu_desc_handle
            );

        SetHookWinProc(WndProc);
    }

    {
        const auto hierarchy = engine::Object::Instantiate<Hierarchy>();
        const auto inspector = engine::Object::Instantiate<Inspector>();
        inspector->selected_game_object_ptr = &hierarchy->selected_game_object;

        AddEditorWindow("Inspector", inspector);
        AddEditorWindow("Hierarchy", hierarchy);
        AddEditorWindow("Editor Prefs", engine::Object::Instantiate<EditorPrefs>());
        AddEditorWindow("Profiler", engine::Object::Instantiate<Profiler>());
        AddEditorWindow("Update Manager Debugger", engine::Object::Instantiate<UpdateManDebugger>());
    }
}

void Editor::Update()
{
}

void Editor::Attach()
{
    engine::UpdateManager::SubscribeDrawCall(shared_from_base<Editor>());
}

void Editor::OnDraw()
{
    {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();
    }

    if (EditorPrefs::theme != m_last_editor_style_)
        SetEditorStyle(EditorPrefs::theme);
    if (EditorPrefs::show_grid)
        DxLibHelper::DrawYPlaneGrid();

    for (const auto &window : m_editor_windows_ | std::views::values)
    {
        window->DrawGui();
    }

    {
        ImGui::Render();
        auto draw_data = ImGui::GetDrawData();
        auto cmd_list = g_RenderEngine->CommandList();
        g_RenderEngine->CommandList()->SetDescriptorHeaps(1, &m_pD3DSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(draw_data, cmd_list);
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    RefreshDxLibDirect3DSetting();
}

void Editor::Finalize()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX12_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void Editor::AddEditorWindow(const std::string &name, std::shared_ptr<IEditorWindow> window)
{
    window->editor = this;
    m_editor_windows_.emplace(name, window);
}

std::vector<std::string> Editor::GetEditorWindowNames()
{
    auto keys = std::views::keys(m_editor_windows_);
    return {keys.begin(), keys.end()};
}

std::shared_ptr<IEditorWindow> Editor::GetEditorWindow(const std::string &name)
{
    return m_editor_windows_.at(name);
}

void Editor::RemoveEditorWindow(const std::string &name)
{
    m_editor_windows_.erase(name);
}
}