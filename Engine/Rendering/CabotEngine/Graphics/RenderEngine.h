#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class DescriptorHandle;

class RenderEngine
{
public:
    enum { FRAME_BUFFER_COUNT = 2 };

private:
    static RenderEngine *m_instance_;

    HWND m_h_wnd_;
    UINT m_frame_buffer_width_ = 0;
    UINT m_frame_buffer_height_ = 0;
    UINT m_current_back_buffer_index_ = 0;
    float m_back_ground_color_[4] = {0.5f, 0.5f, 0.5f, 0.5f};

    ComPtr<ID3D12Device6> m_p_device_ = nullptr;
    ComPtr<ID3D12CommandQueue> m_p_queue_ = nullptr;
    ComPtr<IDXGISwapChain3> m_p_swap_chain_ = nullptr;
    ComPtr<ID3D12CommandAllocator> m_p_allocator_[FRAME_BUFFER_COUNT] = {nullptr};
    ComPtr<ID3D12GraphicsCommandList> m_p_command_list_ = nullptr;
    HANDLE m_fence_event_ = nullptr;
    ComPtr<ID3D12Fence> m_pFence = nullptr;
    UINT64 m_fence_value_[FRAME_BUFFER_COUNT] = {};
    D3D12_VIEWPORT m_viewport_ = {};
    D3D12_RECT m_scissor_ = {};

    UINT m_rtv_descriptor_size_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_p_rtv_heap_ = nullptr;
    ComPtr<ID3D12Resource> m_p_render_targets_[FRAME_BUFFER_COUNT] = {nullptr};

    std::shared_ptr<engine::VertexBuffer> m_p_vert_buff_;
    std::shared_ptr<engine::IndexBuffer> m_p_index_buff_;

    UINT m_dsv_descriptor_size_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_p_dsv_heap_ = nullptr;
    ComPtr<ID3D12Resource> m_p_depth_stencil_buffer_ = nullptr;

    ID3D12Resource *m_currentRenderTarget = nullptr;

    bool CreateDevice();
    bool CreateCommandQueue();
    bool CreateSwapChain();
    bool CreateCommandList();
    bool CreateFence();
    void CreateViewPort();
    void CreateScissorRect();
    bool CreateRenderTarget();
    bool CreateDepthStencil();

public:
    static bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight);

    static void BeginRender();
    static void EndRender();
    static void WaitRender();

    static ID3D12Device6 *Device()
    {
        return m_instance_->m_p_device_.Get();
    }

    static ID3D12GraphicsCommandList *CommandList()
    {
        return m_instance_->m_p_command_list_.Get();
    }

    static ID3D12CommandQueue *CommandQueue()
    {
        return m_instance_->m_p_queue_.Get();
    }

    static UINT CurrentBackBufferIndex()
    {
        return m_instance_->m_current_back_buffer_index_;
    }

    static D3D12_VIEWPORT ViewPort()
    {
        return m_instance_->m_viewport_;
    }

    static void SetBackGroundColor(Color color);
};