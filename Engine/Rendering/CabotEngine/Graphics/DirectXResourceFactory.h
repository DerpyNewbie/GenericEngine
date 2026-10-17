#pragma once
#include "ComPtr.h"
#include "RenderEngine.h"

/// <summary>
/// GPUリソースの作成と、GPUが使用中の可能性があるリソースの遅延解放を行うクラスです。
/// </summary>
class DirectXResourceFactory
{
    friend class RenderEngine;

    inline static std::array<std::list<ComPtr<ID3D12Resource>>, RenderEngine::kFrame_Buffer_Count> m_stored_resources_;

    /// <summary>
    /// 指定したバックバッファで解放待ちになっているリソースを破棄します。そのフレームのGPU処理が完了した後に呼び出されます。
    /// </summary>
    /// <param name="back_buffer_index">バックバッファのインデックス</param>
    static void ClearStoredResources(uint32_t back_buffer_index);
    
public:
    /// <summary>
    /// CommittedResourceを作成します。
    /// </summary>
    /// <param name="heap_prop">ヒープの設定</param>
    /// <param name="res_desc">リソースの設定</param>
    /// <param name="init_state">初期状態</param>
    /// <param name="heap_flags">ヒープのフラグ</param>
    /// <param name="clear_value">最適化用のクリア値</param>
    /// <returns>作成したリソース。失敗した場合はnullptr</returns>
    [[nodiscard]] static ComPtr<ID3D12Resource> CreateBuffer(const D3D12_HEAP_PROPERTIES &heap_prop, const D3D12_RESOURCE_DESC &res_desc,
        D3D12_RESOURCE_STATES init_state, D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE, const D3D12_CLEAR_VALUE *clear_value = nullptr);

    /// <summary>
    /// Defaultヒープにバッファを作成し、Uploadヒープ経由でデータをコピーするコマンドを積みます。Uploadバッファは現在のフレームの終了後に解放されます。
    /// </summary>
    /// <param name="data">コピーするデータ</param>
    /// <param name="size">データのサイズ(byte)</param>
    /// <param name="heap_flags">ヒープのフラグ</param>
    /// <param name="initial_state">コピー後に遷移させる状態</param>
    /// <param name="clear_value">最適化用のクリア値</param>
    /// <returns>作成したDefaultヒープのリソース。失敗した場合はnullptr</returns>
    [[nodiscard]] static ComPtr<ID3D12Resource> CreateUploadedBuffer(const void *data, size_t size, D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_GENERIC_READ, const D3D12_CLEAR_VALUE *clear_value = nullptr);

    /// <summary>
    /// リソースを解放待ちリストに追加し、現在のフレームのGPU処理が完了してから解放されるようにします。
    /// </summary>
    /// <param name="resource">解放するリソース</param>
    static void ReleaseResource(ComPtr<ID3D12Resource> resource);
};