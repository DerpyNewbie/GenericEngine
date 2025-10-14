#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class DescriptorHandle;

class RenderEngine
{
    friend class engine::Engine;

public:
    enum { kFrame_Buffer_Count = 2 };

private:
    HWND m_h_wnd_ = nullptr;
    UINT m_current_back_buffer_index_ = 0;
    Color m_background_color_ = {0.5f, 0.5f, 0.5f, 0.5f};

    ComPtr<ID3D12Device6> m_p_device_ = nullptr;
    ComPtr<ID3D12CommandQueue> m_p_queue_ = nullptr;
    ComPtr<IDXGISwapChain3> m_p_swap_chain_ = nullptr;
    ComPtr<ID3D12CommandAllocator> m_p_allocator_[kFrame_Buffer_Count] = {nullptr};
    ComPtr<ID3D12GraphicsCommandList> m_p_command_list_ = nullptr;
    HANDLE m_fence_event_ = nullptr;
    ComPtr<ID3D12Fence> m_p_fence_ = nullptr;
    UINT64 m_fence_value_[kFrame_Buffer_Count] = {};
    D3D12_VIEWPORT m_viewport_ = {};
    D3D12_RECT m_scissor_ = {};

    UINT m_rtv_descriptor_size_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_p_rtv_heap_ = nullptr;
    ComPtr<ID3D12Resource> m_p_render_targets_[kFrame_Buffer_Count] = {nullptr};

    std::shared_ptr<engine::VertexBuffer> m_p_vert_buff_;
    std::shared_ptr<engine::IndexBuffer> m_p_index_buff_;

    UINT m_dsv_descriptor_size_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_p_dsv_heap_ = nullptr;
    ComPtr<ID3D12Resource> m_p_depth_stencil_buffer_ = nullptr;

    ID3D12Resource *m_current_render_target_ = nullptr;

    bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight);

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
    static RenderEngine *Instance();

    void BeginRender();
    void SetMainRenderTarget(Color background_color);
    void SetRenderTarget(ID3D12DescriptorHeap *rtv_heap, ID3D12DescriptorHeap *dsv_heap,
                         Color background_color) const;
    void EndRender();
    void WaitRender();

    static ID3D12Device6 *Device()
    {
        return Instance()->m_p_device_.Get();
    }

    static ID3D12GraphicsCommandList *CommandList()
    {
        return Instance()->m_p_command_list_.Get();
    }

    static ID3D12CommandQueue *CommandQueue()
    {
        return Instance()->m_p_queue_.Get();
    }

    static UINT CurrentBackBufferIndex()
    {
        return Instance()->m_current_back_buffer_index_;
    }

    static D3D12_VIEWPORT Viewport()
    {
        return Instance()->m_viewport_;
    }

    static D3D12_RESOURCE_DESC BBuffDesc()
    {
        return Instance()->m_p_render_targets_[Instance()->m_current_back_buffer_index_]->GetDesc();
    }

    static D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc()
    {
        return Instance()->m_p_rtv_heap_->GetDesc();
    }

    void SetBackgroundColor(Color color);
};