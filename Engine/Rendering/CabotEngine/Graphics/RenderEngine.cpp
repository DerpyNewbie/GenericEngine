#include "pch.h"

#include "RenderEngine.h"

#include "PSOManager.h"
#include "RootSignatureManager.h"
#include "application.h"
#include "update_manager.h"
#include "Rendering/FontData.h"

#include <DirectXColors.h>

RenderEngine *g_RenderEngine;

bool RenderEngine::Init(HWND hwnd, UINT windowWidth, UINT windowHeight)
{
    m_FrameBufferWidth = windowWidth;
    m_FrameBufferHeight = windowHeight;
    m_hWnd = hwnd;

    if (!CreateDevice())
    {
        printf("デバイスの生成に失敗");
        return false;
    }
    if (!CreateCommandQueue())
    {
        printf("コマンドキューの生成に失敗");
        return false;
    }
    if (!CreateSwapChain())
    {
        printf("スワップチェインの生成に失敗");
        return false;
    }
    if (!CreateCommandList())
    {
        printf("コマンドリストの生成に失敗");
        return false;
    }
    if (!CreateFence())
    {
        printf("フェンスの生成に失敗");
        return false;
    }
    CreateViewPort();
    CreateScissorRect();
    if (!CreateRenderTarget())
    {
        printf("レンダーターゲットの生成に失敗");
        return false;
    }
    if (!CreateDepthStencil())
    {
        printf("デプスステンシルバッファの生成に失敗\n");
        return false;
    }
    if (!CreatePeraResource())
    {
        printf("ペラリソースの生成に失敗\n");
        return false;
    }

    printf("描画エンジンの初期化に成功\n");
    return true;
}

void RenderEngine::BeginRender()
{
    // 現在のレンダーターゲットを更新
    m_currentRenderTarget = m_pRenderTargets[m_CurrentBackBufferIndex].Get();

    // コマンドを初期化してためる準備をする
    m_pAllocator[m_CurrentBackBufferIndex]->Reset();
    m_pCommandList->Reset(m_pAllocator[m_CurrentBackBufferIndex].Get(), nullptr);

    // ビューポートとシザー矩形を設定
    m_pCommandList->RSSetViewports(1, &m_Viewport);
    m_pCommandList->RSSetScissorRects(1, &m_Scissor);

    auto currentRtvHandle = m_pPeraRTVHeap->GetCPUDescriptorHandleForHeapStart();
    auto currentDsvHandle = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();
    auto dsBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pDepthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
        );
    m_pCommandList->ResourceBarrier(1, &dsBarrier);

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pPeraResource.Get(), D3D12_RESOURCE_STATE_PRESENT,
                                                        D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_pCommandList->ResourceBarrier(1, &barrier);

    // レンダーターゲットを設定
    m_pCommandList->OMSetRenderTargets(1, &currentRtvHandle, FALSE, &currentDsvHandle);

    // レンダーターゲットをクリア
    constexpr float clearColor[] = {0.5f, 0.5f, 0.5f, 0.5f};
    m_pCommandList->ClearRenderTargetView(currentRtvHandle, clearColor, 0, nullptr);

    // 深度ステンシルビューをクリア
    m_pCommandList->ClearDepthStencilView(currentDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderEngine::DrawPera()
{
    // 現在のフレームのレンダーターゲットビューのディスクリプタヒープの開始アドレスを取得
    auto currentRtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
    currentRtvHandle.ptr += m_CurrentBackBufferIndex * m_RtvDescriptorSize;

    // 深度ステンシルのディスクリプタヒープの開始アドレス取得
    auto currentDsvHandle = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();

    // レンダーターゲットが使用可能になるまで待つ
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_currentRenderTarget, D3D12_RESOURCE_STATE_PRESENT,
                                                        D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_pCommandList->ResourceBarrier(1, &barrier);

    // ビューポートとシザー矩形を設定
    m_pCommandList->RSSetViewports(1, &m_Viewport);
    m_pCommandList->RSSetScissorRects(1, &m_Scissor);

    // レンダーターゲットを設定
    m_pCommandList->OMSetRenderTargets(1, &currentRtvHandle, FALSE, &currentDsvHandle);

    // レンダーターゲットをクリア
    const float clearColor[] = {0.5f, 0.5f, 0.5f, 0.5f};
    m_pCommandList->ClearRenderTargetView(currentRtvHandle, clearColor, 0, nullptr);

    // 深度ステンシルビューをクリア
    m_pCommandList->ClearDepthStencilView(currentDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    DrawPeraPolygon();
}

void RenderEngine::EndRender()
{
    auto sprite_batch = engine::FontData::SpriteBatch();
    auto sprite_font = engine::FontData::SpriteFont();
    sprite_batch->Begin(m_pCommandList.Get());
    sprite_font->DrawString(sprite_batch.get(), "HelloWorld", DirectX::XMFLOAT2(102, 102), DirectX::Colors::Black);
    sprite_batch->End();

    DrawPera();
    // レンダーターゲットに書き込み終わるまで待つ
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pPeraResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                        D3D12_RESOURCE_STATE_PRESENT);
    m_pCommandList->ResourceBarrier(1, &barrier);
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_currentRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                   D3D12_RESOURCE_STATE_PRESENT);
    m_pCommandList->ResourceBarrier(1, &barrier);
    auto dsBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pDepthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_COMMON
        );
    m_pCommandList->ResourceBarrier(1, &dsBarrier);

    // コマンドの記録を終了
    m_pCommandList->Close();

    // コマンドを実行
    ID3D12CommandList *ppCmdLists[] = {m_pCommandList.Get()};
    m_pQueue->ExecuteCommandLists(1, ppCmdLists);

    // スワップチェーンを切り替え
    m_pSwapChain->Present(1, 0);
    engine::FontData::GMamory()->Commit(m_pQueue.Get());

    // 描画完了を待つ
    WaitRender();

    // バックバッファ番号更新
    m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

bool RenderEngine::CreateDevice()
{
    auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf()));
    return SUCCEEDED(hr);
}

bool RenderEngine::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    auto hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.ReleaseAndGetAddressOf()));

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
    desc.BufferDesc.Width = m_FrameBufferWidth;
    desc.BufferDesc.Height = m_FrameBufferHeight;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = FRAME_BUFFER_COUNT;
    desc.OutputWindow = m_hWnd;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // スワップチェインの生成
    IDXGISwapChain *pSwapChain = nullptr;
    hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);
    if (FAILED(hr))
    {
        pFactory->Release();
        return false;
    }

    // IDXGISwapChain3を取得
    hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        pFactory->Release();
        pSwapChain->Release();
        return false;
    }

    // バックバッファ番号を取得
    m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

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
        hr = m_pDevice->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_pAllocator[i].ReleaseAndGetAddressOf()));
    }

    if (FAILED(hr))
    {
        return false;
    }

    // コマンドリストの生成
    hr = m_pDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_pAllocator[m_CurrentBackBufferIndex].Get(),
        nullptr,
        IID_PPV_ARGS(&m_pCommandList)
        );

    if (FAILED(hr))
    {
        return false;
    }

    //コマンドリストは開かれている状態で作成されるので、いったん閉じる。
    m_pCommandList->Close();

    return true;
}

bool RenderEngine::CreateFence()
{
    for (auto i = 0u; i < FRAME_BUFFER_COUNT; i++)
    {
        m_fenceValue[i] = 0;
    }

    auto hr = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    m_fenceValue[m_CurrentBackBufferIndex]++;

    //同期を行うときのイベントハンドラを作成する。
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    return m_fenceEvent != nullptr;
}

void RenderEngine::CreateViewPort()
{
    m_Viewport.TopLeftX = 0;
    m_Viewport.TopLeftY = 0;
    m_Viewport.Width = static_cast<float>(m_FrameBufferWidth);
    m_Viewport.Height = static_cast<float>(m_FrameBufferHeight);
    m_Viewport.MinDepth = 0.0f;
    m_Viewport.MaxDepth = 1.0f;
}

void RenderEngine::CreateScissorRect()
{
    m_Scissor.left = 0;
    m_Scissor.right = m_FrameBufferWidth;
    m_Scissor.top = 0;
    m_Scissor.bottom = m_FrameBufferHeight;
}

bool RenderEngine::CreateRenderTarget()
{
    // RTV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = FRAME_BUFFER_COUNT;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pRtvHeap.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    // ディスクリプタのサイズを取得。
    m_RtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
    {
        m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pRenderTargets[i].ReleaseAndGetAddressOf()));
        m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), &rtvDesc, rtvHandle);
        rtvHandle.ptr += m_RtvDescriptorSize;
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
    auto hr = m_pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pDsvHeap));
    if (FAILED(hr))
    {
        return false;
    }

    //ディスクリプタのサイズを取得
    m_DsvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    D3D12_CLEAR_VALUE dsvClearValue;
    dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    dsvClearValue.DepthStencil.Depth = 1.0f;
    dsvClearValue.DepthStencil.Stencil = 0;

    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resourceDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        m_FrameBufferWidth,
        m_FrameBufferHeight,
        1,
        1,
        DXGI_FORMAT_D32_FLOAT,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
    hr = m_pDevice->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &dsvClearValue,
        IID_PPV_ARGS(m_pDepthStencilBuffer.ReleaseAndGetAddressOf())
        );

    if (FAILED(hr))
    {
        return false;
    }

    //ディスクリプタを作成
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();

    m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, dsvHandle);

    return true;
}

bool RenderEngine::CreatePeraResource()
{
    auto heap_desc = m_pRtvHeap->GetDesc();

    auto &bbuff = m_pRenderTargets[m_CurrentBackBufferIndex];
    auto resDesc = bbuff->GetDesc();

    D3D12_HEAP_PROPERTIES heap_prop =
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    constexpr float clearColor[] = {0.5f, 0.5f, 0.5f, 0.5f};
    D3D12_CLEAR_VALUE clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

    auto hr = m_pDevice->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clear_value,
        IID_PPV_ARGS(m_pPeraResource.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    //create rtv heap
    heap_desc.NumDescriptors = 1;
    hr = m_pDevice->CreateDescriptorHeap(
        &heap_desc,
        IID_PPV_ARGS(m_pPeraRTVHeap.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
    rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    m_pDevice->CreateRenderTargetView(
        m_pPeraResource.Get(),
        &rtv_desc,
        m_pPeraRTVHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Format = rtv_desc.Format;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    g_DescriptorHeapManager.Initialize();
    m_pPeraTexHandle = g_DescriptorHeapManager.Get().Allocate();

    m_pDevice->CreateShaderResourceView(
        m_pPeraResource.Get(),
        &srv_desc,
        m_pPeraTexHandle->HandleCPU
        );

    return true;
}

void RenderEngine::DrawPeraPolygon()
{

    if (!m_pVertBuff)
    {
        std::array<Vertex, 4> vertices;
        vertices[0].vertex = Vector3(-1, -1, 0.1f); // 左上
        vertices[1].vertex = Vector3(-1, 1, 0.1f); // 左下
        vertices[2].vertex = Vector3(1, -1, 0.1f); // 右上
        vertices[3].vertex = Vector3(1, 1, 0.1f); // 右下

        vertices[0].uvs[0] = Vector2(0, 1); // 左下
        vertices[1].uvs[0] = Vector2(0, 0); // 左上
        vertices[2].uvs[0] = Vector2(1, 1); // 右下
        vertices[3].uvs[0] = Vector2(1, 0); // 右上
        m_pVertBuff = std::make_shared<engine::VertexBuffer>(vertices.size(), vertices.data());
    }
    if (!m_pIndexBuff)
    {
        std::array<uint32_t, 8> indices;
        indices = {0, 1, 2, 1, 3, 2};
        m_pIndexBuff = std::make_shared<engine::IndexBuffer>(indices.size() * sizeof(uint32_t), indices.data());
    }

    auto cmd_list = m_pCommandList;

    g_RenderEngine->CommandList()->SetGraphicsRootSignature(g_RootSignatureManager.Get("Basic"));
    auto vbView = m_pVertBuff->View();
    auto ibview = m_pIndexBuff->View();
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, &vbView);
    cmd_list->IASetIndexBuffer(&ibview);
    cmd_list->SetPipelineState(g_PSOManager.Get("2DBasic"));
    cmd_list->SetGraphicsRootDescriptorTable(engine::kPixelSRV, m_pPeraTexHandle->HandleGPU);

    cmd_list->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void RenderEngine::WaitRender()
{
    //描画終了待ち
    const UINT64 fenceValue = m_fenceValue[m_CurrentBackBufferIndex];
    m_pQueue->Signal(m_pFence.Get(), fenceValue);
    m_fenceValue[m_CurrentBackBufferIndex]++;

    // 次のフレームの描画準備がまだであれば待機する.
    if (m_pFence->GetCompletedValue() < fenceValue)
    {
        // 完了時にイベントを設定.
        auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent);
        if (FAILED(hr))
        {
            return;
        }

        // 待機処理.
        if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE))
        {
            return;
        }
    }
}