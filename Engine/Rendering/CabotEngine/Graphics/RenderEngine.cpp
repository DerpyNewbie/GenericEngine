#include "pch.h"

#include "RenderEngine.h"
#include "PSOManager.h"
#include "RootSignature.h"
#include "application.h"
#include "Rendering/font_data.h"

bool RenderEngine::Init(HWND hwnd, UINT windowWidth, UINT windowHeight)
{
    ID3D12Debug *debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }

    m_frame_buffer_width_ = windowWidth;
    m_frame_buffer_height_ = windowHeight;
    m_h_wnd_ = hwnd;

    if (!CreateDevice())
    {
        engine::Logger::Error<RenderEngine>("Failed to create Device");
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

    engine::Logger::Log<RenderEngine>("Rendering engine initialization successful");
    PSOManager::Initialize();
    return true;
}

void RenderEngine::BeginRender()
{
    // 現在のレンダーターゲットを更新
    m_currentRenderTarget = m_p_render_targets_[m_current_back_buffer_index_].
        Get();

    // コマンドを初期化してためる準備をする
    m_p_allocator_[m_current_back_buffer_index_]->Reset();
    m_p_command_list_->Reset(m_p_allocator_[m_current_back_buffer_index_].Get(),
                             nullptr);

    // ビューポートとシザー矩形を設定
    m_p_command_list_->RSSetViewports(1, &m_viewport_);
    m_p_command_list_->RSSetScissorRects(1, &m_scissor_);

    auto currentRtvHandle = m_p_rtv_heap_->GetCPUDescriptorHandleForHeapStart();
    currentRtvHandle.ptr += m_current_back_buffer_index_ * m_rtv_descriptor_size_;
    auto currentDsvHandle = m_p_dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    // レンダーターゲットが使用可能になるまで待つ
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_currentRenderTarget,
                                                        D3D12_RESOURCE_STATE_PRESENT,
                                                        D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_p_command_list_->ResourceBarrier(1, &barrier);

    auto dsBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_p_depth_stencil_buffer_.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
        );
    m_p_command_list_->ResourceBarrier(1, &dsBarrier);

    // レンダーターゲットを設定
    m_p_command_list_->OMSetRenderTargets(1, &currentRtvHandle, FALSE, &currentDsvHandle);

    // レンダーターゲットをクリア
    m_p_command_list_->ClearRenderTargetView(currentRtvHandle, m_background_color_, 0,
                                             nullptr);

    // 深度ステンシルビューをクリア
    m_p_command_list_->
        ClearDepthStencilView(currentDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    CommandList()->SetGraphicsRootSignature(engine::RootSignature::Get());
    auto descriptor_heap = DescriptorHeap::GetHeap();
    CommandList()->SetDescriptorHeaps(1, &descriptor_heap);
}

void RenderEngine::EndRender()
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_currentRenderTarget,
                                                        D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                        D3D12_RESOURCE_STATE_PRESENT);
    m_p_command_list_->ResourceBarrier(1, &barrier);
    auto dsBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_p_depth_stencil_buffer_.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_COMMON
        );
    m_p_command_list_->ResourceBarrier(1, &dsBarrier);

    // コマンドの記録を終了
    m_p_command_list_->Close();

    // コマンドを実行
    ID3D12CommandList *ppCmdLists[] = {m_p_command_list_.Get()};
    m_p_queue_->ExecuteCommandLists(1, ppCmdLists);

    // スワップチェーンを切り替え
    m_p_swap_chain_->Present(1, 0);
    if (const auto graphics_memory = engine::FontData::GraphicsMemory())
        graphics_memory->Commit(m_p_queue_.Get());

    // 描画完了を待つ
    WaitRender();

    // バックバッファ番号更新
    m_current_back_buffer_index_ = m_p_swap_chain_->GetCurrentBackBufferIndex();
}

void RenderEngine::SetBackgroundColor(Color color)
{
    m_background_color_ = color;
}

bool RenderEngine::CreateDevice()
{
    auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_p_device_.ReleaseAndGetAddressOf()));
    return SUCCEEDED(hr);
}

bool RenderEngine::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    auto hr = m_p_device_->CreateCommandQueue(&desc, IID_PPV_ARGS(m_p_queue_.ReleaseAndGetAddressOf()));

    return SUCCEEDED(hr);
}

bool RenderEngine::CreateSwapChain()
{
    // DXGIファクトリーの生成
    IDXGIFactory4 *pFactory = nullptr;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
    if (FAILED(hr))
    {
        return false;
    }

    // スワップチェインの生成
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = m_frame_buffer_width_;
    desc.BufferDesc.Height = m_frame_buffer_height_;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = FRAME_BUFFER_COUNT;
    desc.OutputWindow = m_h_wnd_;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // スワップチェインの生成
    IDXGISwapChain *pSwapChain = nullptr;
    hr = pFactory->CreateSwapChain(m_p_queue_.Get(), &desc, &pSwapChain);
    if (FAILED(hr))
    {
        pFactory->Release();
        return false;
    }

    // IDXGISwapChain3を取得
    hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_p_swap_chain_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        pFactory->Release();
        pSwapChain->Release();
        return false;
    }

    // バックバッファ番号を取得
    m_current_back_buffer_index_ = m_p_swap_chain_->GetCurrentBackBufferIndex();

    pFactory->Release();
    pSwapChain->Release();
    return true;
}

bool RenderEngine::CreateCommandList()
{
    // コマンドアロケーターの作成
    HRESULT hr;
    for (size_t i = 0; i < FRAME_BUFFER_COUNT; i++)
    {
        hr = m_p_device_->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_p_allocator_[i].ReleaseAndGetAddressOf()));
    }

    if (FAILED(hr))
    {
        return false;
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
    m_p_command_list_->Close();

    return true;
}

bool RenderEngine::CreateFence()
{
    for (auto i = 0u; i < FRAME_BUFFER_COUNT; i++)
    {
        m_fence_value_[i] = 0;
    }

    auto hr = m_p_device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()));
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
    m_viewport_.Width = static_cast<float>(m_frame_buffer_width_);
    m_viewport_.Height = static_cast<float>(m_frame_buffer_height_);
    m_viewport_.MinDepth = 0.0f;
    m_viewport_.MaxDepth = 1.0f;
}

void RenderEngine::CreateScissorRect()
{
    m_scissor_.left = 0;
    m_scissor_.right = m_frame_buffer_width_;
    m_scissor_.top = 0;
    m_scissor_.bottom = m_frame_buffer_height_;
}

bool RenderEngine::CreateRenderTarget()
{
    // RTV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = FRAME_BUFFER_COUNT;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = m_p_device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_p_rtv_heap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    // ディスクリプタのサイズを取得。
    m_rtv_descriptor_size_ = m_p_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_p_rtv_heap_->GetCPUDescriptorHandleForHeapStart();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
    {
        m_p_swap_chain_->GetBuffer(i, IID_PPV_ARGS(m_p_render_targets_[i].ReleaseAndGetAddressOf()));
        m_p_device_->CreateRenderTargetView(m_p_render_targets_[i].Get(), &rtvDesc, rtvHandle);
        rtvHandle.ptr += m_rtv_descriptor_size_;
    }

    return true;
}

bool RenderEngine::CreateDepthStencil()
{
    //DSV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = m_p_device_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_p_dsv_heap_));
    if (FAILED(hr))
    {
        return false;
    }

    //ディスクリプタのサイズを取得
    m_dsv_descriptor_size_ = m_p_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    D3D12_CLEAR_VALUE dsvClearValue;
    dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    dsvClearValue.DepthStencil.Depth = 1.0f;
    dsvClearValue.DepthStencil.Stencil = 0;

    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resourceDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        m_frame_buffer_width_,
        m_frame_buffer_height_,
        1,
        1,
        DXGI_FORMAT_D32_FLOAT,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
    hr = m_p_device_->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &dsvClearValue,
        IID_PPV_ARGS(m_p_depth_stencil_buffer_.ReleaseAndGetAddressOf())
        );

    if (FAILED(hr))
    {
        return false;
    }

    //ディスクリプタを作成
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_p_dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    m_p_device_->CreateDepthStencilView(m_p_depth_stencil_buffer_.Get(), nullptr, dsvHandle);

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
    const UINT64 fenceValue = m_fence_value_[m_current_back_buffer_index_];
    m_p_queue_->Signal(m_pFence.Get(), fenceValue);
    m_fence_value_[m_current_back_buffer_index_]++;

    // 次のフレームの描画準備がまだであれば待機する.
    if (m_pFence->GetCompletedValue() < fenceValue)
    {
        // 完了時にイベントを設定.
        auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_fence_event_);
        if (FAILED(hr))
        {
            return;
        }

        // 待機処理.
        if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_fence_event_, INFINITE, FALSE))
        {
            return;
        }
    }
}