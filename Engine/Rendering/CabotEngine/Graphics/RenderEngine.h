#pragma once

#pragma comment(lib, "d3d12.lib") // d3d12ライブラリをリンクする
#pragma comment(lib, "dxgi.lib") // dxgiライブラリをリンクする
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class DescriptorHandle;

class RenderEngine
{
public:
    enum { FRAME_BUFFER_COUNT = 2 }; // ダブルバッファリングするので2

public:
    bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight); // エンジン初期化

    void BeginRender(); // 描画の開始処理
    void SetMainRenderTarget(Color background_color);
    void SetRenderTarget(ID3D12DescriptorHeap *rtv_heap, Color background_color) const;
    void EndRender(); // 描画の終了処理
    void WaitRender(); // 描画完了を待つ処理

    // 外からアクセスしたいのでGetterとして公開するもの
    ID3D12Device6 *Device()
    {
        return m_pDevice.Get();
    }

    ID3D12GraphicsCommandList *CommandList()
    {
        return m_pCommandList.Get();
    }

    ID3D12CommandQueue *CommandQueue()
    {
        return m_pQueue.Get();
    }

    UINT CurrentBackBufferIndex()
    {
        return m_CurrentBackBufferIndex;
    }

    D3D12_VIEWPORT ViewPort() const
    {
        return m_Viewport;
    }

    D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc() const
    {
        return m_pRtvHeap->GetDesc();
    }

    D3D12_RESOURCE_DESC BBuffDesc() const
    {
        return m_pRenderTargets[m_CurrentBackBufferIndex]->GetDesc();
    }

private:
    // DirectX12初期化に使う関数たち
    bool CreateDevice(); // デバイスを生成
    bool CreateCommandQueue(); // コマンドキューを生成
    bool CreateSwapChain(); // スワップチェインを生成
    bool CreateCommandList(); // コマンドリストとコマンドアロケーターを生成
    bool CreateFence(); // フェンスを生成
    void CreateViewPort(); // ビューポートを生成
    void CreateScissorRect(); // シザー矩形を生成

    // 描画に使うDirectX12のオブジェクトたち
    HWND m_hWnd;
    UINT m_FrameBufferWidth = 0;
    UINT m_FrameBufferHeight = 0;
    UINT m_CurrentBackBufferIndex = 0;

    ComPtr<ID3D12Device6> m_pDevice = nullptr; // デバイス
    ComPtr<ID3D12CommandQueue> m_pQueue = nullptr; // コマンドキュー
    ComPtr<IDXGISwapChain3> m_pSwapChain = nullptr; // スワップチェイン
    ComPtr<ID3D12CommandAllocator> m_pAllocator[FRAME_BUFFER_COUNT] = {nullptr}; // コマンドアロケーたー
    ComPtr<ID3D12GraphicsCommandList> m_pCommandList = nullptr; // コマンドリスト
    HANDLE m_fenceEvent = nullptr; // フェンスで使うイベント
    ComPtr<ID3D12Fence> m_pFence = nullptr; // フェンス
    UINT64 m_fenceValue[FRAME_BUFFER_COUNT]; // フェンスの値（ダブルバッファリング用に2個）
    D3D12_VIEWPORT m_Viewport; // ビューポート
    D3D12_RECT m_Scissor; // シザー矩形

    // 描画に使うオブジェクトとその生成関数たち
    bool CreateRenderTarget(); // レンダーターゲットを生成
    bool CreateDepthStencil(); // 深度ステンシルバッファを生成

    UINT m_RtvDescriptorSize = 0; // レンダーターゲットビューのディスクリプタサイズ
    ComPtr<ID3D12DescriptorHeap> m_pRtvHeap = nullptr; // レンダーターゲットのディスクリプタヒープ
    ComPtr<ID3D12Resource> m_pRenderTargets[FRAME_BUFFER_COUNT] = {nullptr}; // レンダーターゲット（ダブルバッファリングするので2個）

    std::shared_ptr<engine::VertexBuffer> m_pVertBuff;
    std::shared_ptr<engine::IndexBuffer> m_pIndexBuff;

    UINT m_DsvDescriptorSize = 0; // 深度ステンシルのディスクリプターサイズ
    ComPtr<ID3D12DescriptorHeap> m_pDsvHeap = nullptr; // 深度ステンシルのディスクリプタヒープ
    ComPtr<ID3D12Resource> m_pDepthStencilBuffer = nullptr; // 深度ステンシルバッファ（こっちは1つでいい

    // 描画ループで使用するもの
    ID3D12Resource *m_currentRenderTarget = nullptr; // 現在のフレームのレンダーターゲットを一時的に保存しておく関数
};

extern RenderEngine *g_RenderEngine; // どこからでも参照したいのでグローバルにする