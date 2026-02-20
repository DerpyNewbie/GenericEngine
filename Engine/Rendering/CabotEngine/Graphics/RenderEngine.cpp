#include "pch.h"

#include "RenderEngine.h"
#include "PSOManager.h"
#include "application.h"
#include "Rendering/font_data.h"

bool RenderEngine::Init(HWND hwnd, UINT windowWidth, UINT windowHeight)
{
    ID3D12Debug *debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }

    m_h_wnd_ = hwnd;

    if (!CreateDevice())
    {
        engine::Logger::Error<RenderEngine>("Failed to create Device");
        return false;
    }
    if (!CreateDxrDevice())
    {
        engine::Logger::Error<RenderEngine>("Failed to create Dxr Device");
        return false;
    }
    if (!CreateCommandQueue())
    {
        engine::Logger::Error<RenderEngine>("Failed to create CommandQueue");
        return false;
    }
    if (!CreateSwapChain())
    {
        engine::Logger::Error<RenderEngine>("Failed to create SwapChain");
        return false;
    }
    if (!CreateCommandList())
    {
        engine::Logger::Error<RenderEngine>("Failed to create CommandList");
        return false;
    }
    if (!CreateDxrCommandList())
    {
        engine::Logger::Error<RenderEngine>("Failed to create DxrCommandList");
        return false;
    }
        
    if (!CreateFence())
    {
        engine::Logger::Error<RenderEngine>("Failed to create CreateFence");
        return false;
    }
    CreateViewPort();
    CreateScissorRect();
    if (!CreateRenderTarget())
    {
        engine::Logger::Error<RenderEngine>("Failed to create MainRenderTarget");
        return false;
    }
    if (!CreateDepthStencil())
    {
        engine::Logger::Error<RenderEngine>("Failed to create DepthStencil");
        return false;
    }

    engine::Application::on_window_resized.AddListener([this] {
        UpdateMainRenderTarget();
    });

    m_can_use_dxr_ = CheckSupportedDxr();

    engine::Logger::Log<RenderEngine>("Rendering engine initialization successful");
    PSOManager::Initialize();
    return true;
}

void RenderEngine::BeginRender()
{
    // 現在のレンダーターゲットを更新
    m_current_render_target_ = m_p_render_targets_[m_current_back_buffer_index_].
        Get();

    // コマンドを初期化してためる準備をする
    m_p_allocator_[m_current_back_buffer_index_]->Reset();
    m_p_command_list_->Reset(m_p_allocator_[m_current_back_buffer_index_].Get(),
                             nullptr);

    const auto ds_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_p_depth_stencil_buffer_.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    );
    m_p_command_list_->ResourceBarrier(1, &ds_barrier);
}

void RenderEngine::SetMainRenderTarget(const Color background_color)
{
    // レンダーターゲットが使用可能になるまで待つ
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_current_render_target_, D3D12_RESOURCE_STATE_PRESENT,
                                                        D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_p_command_list_->ResourceBarrier(1, &barrier);

    // ビューポートとシザー矩形を設定
    m_p_command_list_->RSSetViewports(1, &m_viewport_);
    m_p_command_list_->RSSetScissorRects(1, &m_scissor_);

    auto currentRtvHandle = m_p_rtv_heap_->GetCPUDescriptorHandleForHeapStart();
    currentRtvHandle.ptr += m_current_back_buffer_index_ * m_rtv_descriptor_size_;
    auto currentDsvHandle = m_p_dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    // レンダーターゲットを設定
    m_p_command_list_->OMSetRenderTargets(1, &currentRtvHandle, FALSE, &currentDsvHandle);

    // レンダーターゲットをクリア
    float clearColor[] = { background_color.R(), background_color.G(), background_color.B(), background_color.A() };
    m_p_command_list_->ClearRenderTargetView(currentRtvHandle, clearColor, 0, nullptr);

    // 深度ステンシルビューをクリア
    m_p_command_list_->ClearDepthStencilView(currentDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderEngine::SetRenderTarget(ID3D12DescriptorHeap *rtv_heap, ID3D12DescriptorHeap *dsv_heap,
    const Color background_color, const D3D12_VIEWPORT *viewport, const D3D12_RECT *scissor) const
{
    // 現在のフレームのレンダーターゲットビューのディスクリプタヒープの開始アドレスを取得
    int num_render_target = 0;
    const D3D12_CPU_DESCRIPTOR_HANDLE *current_rtv_handle = nullptr;
    if (rtv_heap != nullptr)
    {
        num_render_target = 1;
        const auto rtv_handle = rtv_heap->GetCPUDescriptorHandleForHeapStart();
        current_rtv_handle = &rtv_handle;
    }

    // 深度ステンシルのディスクリプタヒープの開始アドレス取得
    const D3D12_CPU_DESCRIPTOR_HANDLE current_dsv_handle = dsv_heap == nullptr
        ? m_p_dsv_heap_->GetCPUDescriptorHandleForHeapStart()
        : dsv_heap->GetCPUDescriptorHandleForHeapStart();

    // ビューポートとシザー矩形を設定
    m_p_command_list_->RSSetViewports(1, viewport == nullptr ? &m_viewport_ : viewport);
    m_p_command_list_->RSSetScissorRects(1, scissor == nullptr ? &m_scissor_ : scissor);

    // レンダーターゲットを設定
    m_p_command_list_->OMSetRenderTargets(
        num_render_target, current_rtv_handle, FALSE, &current_dsv_handle
    );

    // レンダーターゲットをクリア
    if (current_rtv_handle != nullptr)
    {
        m_p_command_list_->ClearRenderTargetView(
            *current_rtv_handle, background_color, 0, nullptr
        );
    }

    // 深度ステンシルビューをクリア
    m_p_command_list_->ClearDepthStencilView(
        current_dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr
    );
}

void RenderEngine::EndRender()
{
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_current_render_target_,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );

    m_p_command_list_->ResourceBarrier(1, &barrier);
    const auto ds_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_p_depth_stencil_buffer_.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_COMMON
    );

    m_p_command_list_->ResourceBarrier(1, &ds_barrier);

    // コマンドの記録を終了
    m_p_command_list_->Close();

    // コマンドを実行
    ID3D12CommandList *pp_cmd_lists[] = { m_p_command_list_.Get() };
    m_p_queue_->ExecuteCommandLists(1, pp_cmd_lists);

    // スワップチェーンを切り替え
    m_p_swap_chain_->Present(1, 0);
    if (const auto graphics_memory = engine::FontData::GraphicsMemory())
        graphics_memory->Commit(m_p_queue_.Get());

    // 描画完了を待つ
    WaitRender();

    // バックバッファ番号更新
    m_current_back_buffer_index_ = m_p_swap_chain_->GetCurrentBackBufferIndex();
}

void RenderEngine::SetBackgroundColor(const Color color)
{
    m_background_color_ = color;
}

bool RenderEngine::CreateDevice()
{
    const auto hr = D3D12CreateDevice(
        nullptr, D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(m_p_device_.ReleaseAndGetAddressOf())
    );

    return SUCCEEDED(hr);
}

bool RenderEngine::CreateDxrDevice()
{
    const auto hr = m_p_device_.As(&m_dxr_device_);

    return SUCCEEDED(hr);
}

bool RenderEngine::CheckSupportedDxr() const
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
    auto hr = m_dxr_device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));

    if (FAILED(hr))
        return false;

    return options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0;
}

bool RenderEngine::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    const auto hr = m_p_device_->CreateCommandQueue(&desc, IID_PPV_ARGS(m_p_queue_.ReleaseAndGetAddressOf()));
    return SUCCEEDED(hr);
}

bool RenderEngine::CreateSwapChain()
{
    // DXGIファクトリーの生成
    IDXGIFactory2 *dxgi_factory = nullptr;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory));
    if (FAILED(hr))
    {
        return false;
    }

    // スワップチェインの生成
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = engine::Application::WindowWidth();
    desc.Height = engine::Application::WindowHeight();
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = kFrame_Buffer_Count;
    desc.Scaling = DXGI_SCALING_NONE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // スワップチェインの生成
    IDXGISwapChain1 *swap_chain = nullptr;
    hr = dxgi_factory->CreateSwapChainForHwnd(
        m_p_queue_.Get(), engine::Application::WindowHandle(), &desc, nullptr, nullptr, &swap_chain
    );

    if (FAILED(hr))
    {
        dxgi_factory->Release();
        return false;
    }

    // IDXGISwapChain3を取得
    hr = swap_chain->QueryInterface(IID_PPV_ARGS(m_p_swap_chain_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        dxgi_factory->Release();
        swap_chain->Release();
        return false;
    }

    // バックバッファ番号を取得
    m_current_back_buffer_index_ = m_p_swap_chain_->GetCurrentBackBufferIndex();

    dxgi_factory->Release();
    swap_chain->Release();
    return true;
}

bool RenderEngine::CreateCommandList()
{
    // コマンドアロケーターの作成
    HRESULT hr;
    for (size_t i = 0; i < kFrame_Buffer_Count; i++)
    {
        hr = m_p_device_->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_p_allocator_[i].ReleaseAndGetAddressOf())
        );

        if (FAILED(hr))
        {
            return false;
        }
    }

    // コマンドリストの生成
    hr = m_p_device_->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_p_allocator_[m_current_back_buffer_index_].Get(),
        nullptr,
        IID_PPV_ARGS(&m_p_command_list_)
    );

    if (FAILED(hr))
    {
        return false;
    }

    //コマンドリストは開かれている状態で作成されるので、いったん閉じる。
    auto _ = m_p_command_list_->Close();
    return true;
}

bool RenderEngine::CreateDxrCommandList()
{
    const auto hr = m_p_command_list_.As(&m_dxr_command_list_);

    return SUCCEEDED(hr);
}

bool RenderEngine::CreateFence()
{
    for (unsigned long long &i : m_fence_value_)
    {
        i = 0;
    }

    const auto hr = m_p_device_->CreateFence(
        0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_p_fence_.ReleaseAndGetAddressOf())
    );

    if (FAILED(hr))
    {
        return false;
    }

    m_fence_value_[m_current_back_buffer_index_]++;

    //同期を行うときのイベントハンドラを作成する。
    m_fence_event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    return m_fence_event_ != nullptr;
}

void RenderEngine::CreateViewPort()
{
    m_viewport_.TopLeftX = 0;
    m_viewport_.TopLeftY = 0;
    m_viewport_.Width = static_cast<float>(engine::Application::WindowWidth());
    m_viewport_.Height = static_cast<float>(engine::Application::WindowHeight());
    m_viewport_.MinDepth = 0.0f;
    m_viewport_.MaxDepth = 1.0f;
}

void RenderEngine::CreateScissorRect()
{
    m_scissor_.left = 0;
    m_scissor_.right = engine::Application::WindowWidth();
    m_scissor_.top = 0;
    m_scissor_.bottom = engine::Application::WindowHeight();
}

bool RenderEngine::CreateRenderTarget()
{
    // RTV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = kFrame_Buffer_Count;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    const auto hr = m_p_device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_p_rtv_heap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    // ディスクリプタのサイズを取得。
    m_rtv_descriptor_size_ = m_p_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = m_p_rtv_heap_->GetCPUDescriptorHandleForHeapStart();

    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};

    rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    for (UINT i = 0; i < kFrame_Buffer_Count; i++)
    {
        m_p_swap_chain_->GetBuffer(i, IID_PPV_ARGS(m_p_render_targets_[i].ReleaseAndGetAddressOf()));
        m_p_device_->CreateRenderTargetView(m_p_render_targets_[i].Get(), &rtv_desc, rtv_handle);
        rtv_handle.ptr += m_rtv_descriptor_size_;
    }

    return true;
}

bool RenderEngine::CreateDepthStencil()
{
    //DSV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors = 1;
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = m_p_device_->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_p_dsv_heap_));
    if (FAILED(hr))
    {
        return false;
    }

    //ディスクリプタのサイズを取得
    m_dsv_descriptor_size_ = m_p_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    D3D12_CLEAR_VALUE dsv_clear_value;
    dsv_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_clear_value.DepthStencil.Depth = 1.0f;
    dsv_clear_value.DepthStencil.Stencil = 0;

    const auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const CD3DX12_RESOURCE_DESC resource_desc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        engine::Application::WindowWidth(),
        engine::Application::WindowHeight(),
        1,
        1,
        DXGI_FORMAT_D32_FLOAT,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE
    );

    hr = m_p_device_->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_COMMON,
        &dsv_clear_value,
        IID_PPV_ARGS(m_p_depth_stencil_buffer_.ReleaseAndGetAddressOf())
    );

    m_p_depth_stencil_buffer_->SetName(L"DepthStencilBuffer");

    if (FAILED(hr))
    {
        return false;
    }

    //ディスクリプタを作成
    const D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = m_p_dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    m_p_device_->CreateDepthStencilView(m_p_depth_stencil_buffer_.Get(), nullptr, dsv_handle);

    return true;
}

RenderEngine *RenderEngine::Instance()
{
    static auto instance = new RenderEngine;
    return instance;
}

void RenderEngine::WaitRender()
{
    //描画終了待ち
    const UINT64 fence_value = m_fence_value_[m_current_back_buffer_index_];
    m_p_queue_->Signal(m_p_fence_.Get(), fence_value);
    m_fence_value_[m_current_back_buffer_index_]++;

    // 次のフレームの描画準備がまだであれば待機する.
    if (m_p_fence_->GetCompletedValue() < fence_value)
    {
        // 完了時にイベントを設定.
        const auto hr = m_p_fence_->SetEventOnCompletion(fence_value, m_fence_event_);
        if (FAILED(hr))
        {
            return;
        }

        WaitForSingleObject(m_fence_event_, INFINITE);
    }
}

void RenderEngine::UpdateMainRenderTarget()
{
    engine::Logger::Log<RenderEngine>("Updating main render target");
    WaitRender();

    for (auto rt : m_p_render_targets_)
        rt.Reset();

    m_p_swap_chain_->ResizeBuffers(kFrame_Buffer_Count,
                                   engine::Application::WindowWidth(),
                                   engine::Application::WindowHeight(),
                                   DXGI_FORMAT_UNKNOWN,
                                   0);

    CreateRenderTarget();
    CreateDepthStencil();
    CreateViewPort();
    CreateScissorRect();
}