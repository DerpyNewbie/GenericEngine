#pragma once
#include "Rendering/depth_texture.h"
#include "Rendering/texture_buffer.h"
#include "Rendering/CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
/// <summary>
/// DepthTextureを深度バッファおよびSRVとして扱うためのBufferです。
/// </summary>
class DepthTextureBuffer : public TextureBuffer
{
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;

public:
    /// <summary>
    /// DepthTextureの設定からBufferを生成します。リソースはCreateBufferまたはSetResourceで設定されます。
    /// </summary>
    /// <param name="depth_texture">元となるDepthTexture</param>
    DepthTextureBuffer(const std::shared_ptr<DepthTexture> &depth_texture);

    /// <summary>
    /// Windowサイズの深度リソースとDSV用のDescriptorHeapを作成します。
    /// </summary>
    void CreateBuffer() override;

    /// <summary>
    /// Texture2DArrayの指定した要素を書き込み先とするDSVを作成し、リソースとして設定します。
    /// </summary>
    /// <param name="texture_array">書き込み先のTexture2DArray</param>
    /// <param name="index">書き込み先の要素番号</param>
    void SetResource(const std::shared_ptr<Texture2DArray> &texture_array, int index);

    /// <summary>
    /// DSV用のDescriptorHeapを取得します。
    /// </summary>
    ID3D12DescriptorHeap *GetHeap() const;
    /// <summary>
    /// R32_FLOATのTexture2DとしてのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}