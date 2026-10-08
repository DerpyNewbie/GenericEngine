#pragma once
#include "CabotEngine/Graphics/Texture2D.h"
#include "CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
/// <summary>
/// 深度を書き込むためのTextureです。
/// </summary>
class DepthTexture : public Texture2D
{
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;

public:
    /// <summary>
    /// Windowと同じサイズの深度用のBufferと、DSV用のDescriptorHeapを作成します。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// Bufferを深度を書き込める状態に遷移させます。Bufferがない場合は作成します。
    /// </summary>
    void BeginRender();
    /// <summary>
    /// BufferをPixelShaderから読める状態に戻します。
    /// </summary>
    void EndRender();

    /// <summary>
    /// Texture2DArrayの指定された要素を、深度の書き込み先として使うように設定します。
    /// </summary>
    /// <param name="texture_array">書き込み先のTexture2DArray</param>
    /// <param name="index">使用する要素のindex</param>
    void SetResource(const std::shared_ptr<Texture2DArray> &texture_array, int index);

    /// <summary>
    /// DSV用のDescriptorHeapを取得します。
    /// </summary>
    ID3D12DescriptorHeap *GetHeap();
    /// <summary>
    /// R32_FLOATのTexture2Dとして読むためのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}