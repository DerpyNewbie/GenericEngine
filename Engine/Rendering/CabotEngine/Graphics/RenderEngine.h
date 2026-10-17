#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class DescriptorHandle;

/// <summary>
/// DirectX12のDevice、CommandQueue、SwapChain、メインのレンダーターゲットなどを管理し、フレームの描画の開始と終了を制御するシングルトンクラスです。
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
    /// DebugLayerを有効にし、Device、CommandQueue、SwapChain、CommandList、Fence、レンダーターゲット、深度バッファを作成します。
    /// </summary>
    /// <param name="hwnd">描画先のWindowハンドル</param>
    /// <param name="windowWidth">Windowの幅</param>
    /// <param name="windowHeight">Windowの高さ</param>
    /// <returns>すべての初期化に成功した場合はtrue</returns>
    bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight);

    /// <summary>
    /// D3D12Deviceを作成します。
    /// </summary>
    bool CreateDevice();
    /// <summary>
    /// Direct用のCommandQueueを作成します。
    /// </summary>
    bool CreateCommandQueue();
    /// <summary>
    /// Windowサイズのフレームバッファ数分のSwapChainを作成します。
    /// </summary>
    bool CreateSwapChain();
    /// <summary>
    /// フレームバッファ数分のCommandAllocatorとCommandListを作成します。
    /// </summary>
    bool CreateCommandList();
    /// <summary>
    /// フレーム同期用のFenceとEventを作成します。
    /// </summary>
    bool CreateFence();
    /// <summary>
    /// Windowサイズに合わせたViewportを設定します。
    /// </summary>
    void CreateViewPort();
    /// <summary>
    /// Windowサイズに合わせたシザー矩形を設定します。
    /// </summary>
    void CreateScissorRect();
    /// <summary>
    /// SwapChainの各バッファに対するRTVを作成します。
    /// </summary>
    bool CreateRenderTarget();
    /// <summary>
    /// メインのレンダーターゲット用の深度バッファとDSVを作成します。
    /// </summary>
    bool CreateDepthStencil();

public:
    /// <summary>
    /// RenderEngineの唯一のインスタンスを取得します。
    /// </summary>
    static RenderEngine *Instance();

    /// <summary>
    /// フレームの描画を開始します。CommandAllocatorとCommandListをリセットし、深度バッファを書き込み可能な状態にします。
    /// </summary>
    void BeginRender();
    /// <summary>
    /// 現在のバックバッファを描画先に設定し、指定した色と深度をクリアします。
    /// </summary>
    /// <param name="background_color">クリアする色</param>
    void SetMainRenderTarget(Color background_color);
    /// <summary>
    /// 指定したHeapのRTV/DSVを描画先に設定してクリアします。
    /// </summary>
    /// <param name="rtv_heap">RTVのHeap。nullptrの場合はカラーを書き込みません</param>
    /// <param name="dsv_heap">DSVのHeap。nullptrの場合はメインの深度バッファを使用します</param>
    /// <param name="background_color">クリアする色</param>
    /// <param name="viewport">Viewport。nullptrの場合はメインのViewportを使用します</param>
    /// <param name="scissor">シザー矩形。nullptrの場合はメインのシザー矩形を使用します</param>
    void SetRenderTarget(ID3D12DescriptorHeap *rtv_heap, ID3D12DescriptorHeap *dsv_heap,
        Color background_color, const D3D12_VIEWPORT *viewport = nullptr, const D3D12_RECT *scissor = nullptr) const;
    /// <summary>
    /// フレームの描画を終了します。CommandListを実行してPresentし、次のフレームに進んで解放待ちのリソースを破棄します。
    /// </summary>
    void EndRender();
    /// <summary>
    /// Fenceをシグナルし、次に使用するバックバッファのGPU処理が完了するまで待機します。
    /// </summary>
    void MoveToNextFrame();
    /// <summary>
    /// 現在のGPUの処理がすべて完了するまで待機します。
    /// </summary>
    void WaitRender();
    /// <summary>
    /// GPUの処理を待機した後、現在のWindowサイズに合わせてSwapChain、レンダーターゲット、深度バッファ、Viewportを作り直します。
    /// </summary>
    void UpdateMainRenderTarget();

    /// <summary>
    /// D3D12Deviceを取得します。
    /// </summary>
    static ID3D12Device6 *Device()
    {
        return Instance()->m_p_device_.Get();
    }

    /// <summary>
    /// CommandListを取得します。
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
    /// 現在のバックバッファのインデックスを取得します。
    /// </summary>
    static UINT CurrentBackBufferIndex()
    {
        return Instance()->m_current_back_buffer_index_;
    }

    /// <summary>
    /// メインのViewportを取得します。
    /// </summary>
    static D3D12_VIEWPORT Viewport()
    {
        return Instance()->m_viewport_;
    }

    /// <summary>
    /// 現在のバックバッファのリソースの設定を取得します。
    /// </summary>
    static D3D12_RESOURCE_DESC BBuffDesc()
    {
        return Instance()->m_p_render_targets_[Instance()->m_current_back_buffer_index_]->GetDesc();
    }

    /// <summary>
    /// メインのRTV用DescriptorHeapの設定を取得します。
    /// </summary>
    static D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc()
    {
        return Instance()->m_p_rtv_heap_->GetDesc();
    }

    /// <summary>
    /// 次にシグナルされるFence値を取得します。読み戻しの完了判定に使用します。
    /// </summary>
    static UINT64 FenceNextValue()
    {
        return Instance()->m_next_fence_value_;
    }

    /// <summary>
    /// 指定したFence値までGPUの処理が完了しているかどうかを取得します。
    /// </summary>
    /// <param name="fence_value">判定するFence値</param>
    static bool IsFenceComplete(uint64_t fence_value);

    /// <summary>
    /// 背景色を設定します。
    /// </summary>
    /// <param name="color">背景色</param>
    void SetBackgroundColor(Color color);
};