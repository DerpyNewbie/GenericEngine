#pragma once
#include "render_texture.h"
#include "texture_buffer.h"

namespace engine
{
/// <summary>
/// RenderTextureをレンダーターゲットおよびSRV/UAVとして扱うためのBufferです。
/// </summary>
class RenderTextureBuffer : public TextureBuffer
{
    bool m_is_unordered_access_;
    ComPtr<ID3D12DescriptorHeap> m_rtv_heap_;

public:
    /// <summary>
    /// RenderTextureの設定からBufferを生成します。リソースはCreateBufferで作成されます。
    /// </summary>
    /// <param name="render_texture">元となるRenderTexture</param>
    RenderTextureBuffer(const std::shared_ptr<RenderTexture> &render_texture);

    /// <summary>
    /// レンダーターゲットとして使用可能なリソースと、RTV用のDescriptorHeapを作成します。UAVが許可されている場合はUAVも使用可能にします。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// 指定したDescriptorHandleの位置にSRVまたはUAVを作成します。
    /// </summary>
    /// <param name="desc_handle">Viewを作成するDescriptorHandle</param>
    /// <param name="is_uav">UAVとして作成する場合はtrue</param>
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;

    /// <summary>
    /// RTV用のDescriptorHeapを取得します。
    /// </summary>
    ID3D12DescriptorHeap *GetHeap() const;

    /// <summary>
    /// Texture2DとしてのUAVの設定を取得します。
    /// </summary>
    D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc();
    /// <summary>
    /// Texture2DとしてのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}