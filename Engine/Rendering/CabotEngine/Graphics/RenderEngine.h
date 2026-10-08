#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class DescriptorHandle;

/// <summary>
/// DirectX 12のDevice、SwapChain、CommandListなどを管理し、フレームの描画を行うクラスです。
/// </summary>
class RenderEngine
{
    friend class engine::Engine;

public:
    enum { kFrame_Buffer_Count = 2 };

private:
    HWND m_h_wnd_ = nullptr;
    UINT m_current_back_buffer_index_ = 0;
    Color m_background_color_ = { 0.5f, 0.5f, 0.5f, 0.5f };

    ComPtr<ID3D12Device6> m_p_device_ = nullptr;
    ComPtr<ID3D12CommandQueue> m_p_queue_ = nullptr;
    ComPtr<IDXGISwapChain3> m_p_swap_chain_ = nullptr;
    ComPtr<ID3D12CommandAllocator> m_p_allocator_[kFrame_Buffer_Count] = { nullptr };
    ComPtr<ID3D12GraphicsCommandList> m_p_command_list_ = nullptr;
    HANDLE m_fence_event_ = nullptr;
    ComPtr<ID3D12Fence> m_p_fence_ = nullptr;
    UINT64 m_next_fence_value_ = 1;
    UINT64 m_fence_value_[kFrame_Buffer_Count] = {};
    D3D12_VIEWPORT m_viewport_ = {};
    D3D12_RECT m_scissor_ = {};

    UINT m_rtv_descriptor_size_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_p_rtv_heap_ = nullptr;
    ComPtr<ID3D12Resource> m_p_render_targets_[kFrame_Buffer_Count] = { nullptr };

    std::shared_ptr<engine::VertexBuffer> m_p_vert_buff_;
    std::shared_ptr<engine::IndexBuffer> m_p_index_buff_;

    UINT m_dsv_descriptor_size_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_p_dsv_heap_ = nullptr;
    ComPtr<ID3D12Resource> m_p_depth_stencil_buffer_ = nullptr;

    ID3D12Resource *m_current_render_target_ = nullptr;

    /// <summary>
    /// Device、CommandQueue、SwapChain、CommandList、Fence、RenderTarget、DepthStencilなど、描画に必要なものを作成します。
    /// </summary>
    /// <param name="hwnd">描画先のWindow</param>
    /// <returns>いずれかの作成に失敗した場合 false</returns>
    bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight);

    /// <summary>
    /// DirectX12のDeviceを作成します。
    /// </summary>
    /// <returns>成功した場合 true</returns>
    bool CreateDevice();
    /// <summary>
    /// CommandQueueを作成します。
    /// </summary>
    /// <returns>成功した場合 true</returns>
    bool CreateCommandQueue();
    /// <summary>
    /// WindowのサイズでSwapChainを作成します。
    /// </summary>
    /// <returns>成功した場合 true</returns>
    bool CreateSwapChain();
    /// <summary>
    /// BackBufferごとのCommandAllocatorと、CommandListを作成します。
    /// </summary>
    /// <returns>成功した場合 true</returns>
    bool CreateCommandList();
    /// <summary>
    /// GPUとの同期に使うFenceとEventを作成します。
    /// </summary>
    /// <returns>成功した場合 true</returns>
    bool CreateFence();
    /// <summary>
    /// Window全体を描画範囲とするViewportを設定します。
    /// </summary>
    void CreateViewPort();
    /// <summary>
    /// Window全体を対象とするScissorRectを設定します。
    /// </summary>
    void CreateScissorRect();
    /// <summary>
    /// RTV用のDescriptorHeapと、各BackBufferのRenderTargetViewを作成します。
    /// </summary>
    /// <returns>成功した場合 true</returns>
    bool CreateRenderTarget();
    /// <summary>
    /// DSV用のDescriptorHeapと、Windowのサイズの深度Bufferを作成します。
    /// </summary>
    /// <returns>成功した場合 true</returns>
    bool CreateDepthStencil();

public:
    static RenderEngine *Instance();

    /// <summary>
    /// フレームの描画を開始します。CommandListをResetし、深度Bufferを書き込める状態にします。
    /// </summary>
    void BeginRender();
    /// <summary>
    /// BackBufferを描画先に設定し、背景色と深度をクリアします。
    /// </summary>
    /// <param name="background_color">クリアする色</param>
    void SetMainRenderTarget(Color background_color);
    /// <summary>
    /// 指定されたRTVとDSVを描画先に設定し、クリアします。
    /// </summary>
    /// <param name="rtv_heap">RTVのDescriptorHeap。nullptrの場合は深度のみを描画します。</param>
    /// <param name="dsv_heap">DSVのDescriptorHeap。nullptrの場合は既定の深度Bufferを使います。</param>
    /// <param name="background_color">クリアする色</param>
    /// <param name="viewport">Viewport。nullptrの場合はWindow全体</param>
    /// <param name="scissor">ScissorRect。nullptrの場合はWindow全体</param>
    void SetRenderTarget(ID3D12DescriptorHeap *rtv_heap, ID3D12DescriptorHeap *dsv_heap,
        Color background_color, const D3D12_VIEWPORT *viewport = nullptr, const D3D12_RECT *scissor = nullptr) const;
    /// <summary>
    /// フレームの描画を終了します。CommandListを実行して画面に表示し、次のフレームに進みます。
    /// </summary>
    void EndRender();
    /// <summary>
    /// 次のBackBufferに切り替え、そのBackBufferを使った前回の描画がGPUで完了するまで待機します。
    /// </summary>
    void MoveToNextFrame();
    /// <summary>
    /// これまでに発行したCommandの実行がGPUで完了するまで待機します。
    /// </summary>
    void WaitRender();
    /// <summary>
    /// Windowのサイズの変更に合わせて、BackBuffer、RenderTarget、DepthStencil、Viewportを作り直します。
    /// </summary>
    void UpdateMainRenderTarget();

    /// <summary>
    /// DirectX12のDeviceを取得します。
    /// </summary>
    static ID3D12Device6 *Device()
    {
        return Instance()->m_p_device_.Get();
    }

    /// <summary>
    /// 描画に使うCommandListを取得します。
    /// </summary>
    static ID3D12GraphicsCommandList *CommandList()
    {
        return Instance()->m_p_command_list_.Get();
    }

    /// <summary>
    /// CommandQueueを取得します。
    /// </summary>
    static ID3D12CommandQueue *CommandQueue()
    {
        return Instance()->m_p_queue_.Get();
    }

    /// <summary>
    /// 現在描画先になっているBackBufferのindexを取得します。
    /// </summary>
    static UINT CurrentBackBufferIndex()
    {
        return Instance()->m_current_back_buffer_index_;
    }

    /// <summary>
    /// Window全体のViewportを取得します。
    /// </summary>
    static D3D12_VIEWPORT Viewport()
    {
        return Instance()->m_viewport_;
    }

    /// <summary>
    /// 現在のBackBufferのResourceの設定を取得します。
    /// </summary>
    static D3D12_RESOURCE_DESC BBuffDesc()
    {
        return Instance()->m_p_render_targets_[Instance()->m_current_back_buffer_index_]->GetDesc();
    }

    /// <summary>
    /// BackBuffer用のRTVのDescriptorHeapの設定を取得します。
    /// </summary>
    static D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc()
    {
        return Instance()->m_p_rtv_heap_->GetDesc();
    }

    /// <summary>
    /// 背景色を保持する変数を設定します。
    /// </summary>
    /// <param name="color">背景色</param>
    void SetBackgroundColor(Color color);
};