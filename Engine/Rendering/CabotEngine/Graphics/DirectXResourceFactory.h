#pragma once
#include "ComPtr.h"
#include "RenderEngine.h"

/// <summary>
/// DirectXのResource(Buffer)を作成するクラスです。
/// </summary>
class DirectXResourceFactory
{
    friend class RenderEngine;

    inline static std::array<std::list<ComPtr<ID3D12Resource>>, RenderEngine::kFrame_Buffer_Count> m_stored_resources_;

    /// <summary>
    /// 指定されたBackBufferのフレームで保持していた、Upload用のResourceを解放します。
    /// </summary>
    /// <param name="back_buffer_index">BackBufferのindex</param>
    static void ClearStoredResources(uint32_t back_buffer_index);
    
public:
    /// <summary>
    /// Resourceを作成します。
    /// </summary>
    /// <param name="heap_prop">Heapの設定</param>
    /// <param name="res_desc">Resourceの設定</param>
    /// <param name="init_state">作成時の状態</param>
    /// <returns>作成されたResource。失敗した場合 nullptr</returns>
    [[nodiscard]] static ComPtr<ID3D12Resource> CreateBuffer(const D3D12_HEAP_PROPERTIES &heap_prop, const D3D12_RESOURCE_DESC &res_desc,
        D3D12_RESOURCE_STATES init_state, D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE, const D3D12_CLEAR_VALUE *clear_value = nullptr);

    /// <summary>
    /// データをUpload用のBufferを経由してコピーしたBufferを作成します。Upload用のBufferは、後でClearStoredResourcesが呼ばれるまで保持されます。
    /// </summary>
    /// <param name="data">コピーするデータ</param>
    /// <param name="size">データのサイズ(byte)</param>
    /// <param name="initial_state">コピーした後のBufferの状態</param>
    /// <returns>作成されたBuffer。失敗した場合 nullptr</returns>
    [[nodiscard]] static ComPtr<ID3D12Resource> CreateUploadedBuffer(const void *data, size_t size, D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_GENERIC_READ, const D3D12_CLEAR_VALUE *clear_value = nullptr);
};