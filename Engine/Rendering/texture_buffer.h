#pragma once
#include "buffer_base.h"
#include "shader_resource.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
/// <summary>
/// Texture2DのピクセルデータをGPUにSRVとして渡すためのBufferです。
/// </summary>
class TextureBuffer : public BufferBase, public ShaderResource
{
protected:
    ComPtr<ID3D12Resource> m_resource_ = nullptr;

    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_ = {};
    uint32_t m_width_ = -1;
    uint32_t m_height_ = -1;
    uint16_t m_mip_level_;
    DXGI_FORMAT m_format_;
    D3D12_RESOURCE_STATES m_current_state_;

public:
    /// <summary>
    /// リソースをDirectXResourceFactoryの解放待ちに登録し、GPUの使用が終わってから解放されるようにします。
    /// </summary>
    ~TextureBuffer() override;
    TextureBuffer() = default;
    /// <summary>
    /// Texture2Dからピクセルデータとフォーマット、サイズ、MipLevelを取得してBufferを生成します。リソースはCreateBufferで作成されます。
    /// </summary>
    /// <param name="texture">元となるTexture2D</param>
    TextureBuffer(const std::shared_ptr<Texture2D> &texture);

    /// <summary>
    /// CPUから書き込み可能なヒープにテクスチャリソースを作成し、ピクセルデータを書き込みます。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// テクスチャの更新には対応していないため、エラーを出力します。
    /// </summary>
    void UpdateBuffer(const void *data) override;
    /// <summary>
    /// 指定したDescriptorHandleの位置にSRVを作成します。is_uavは無視されます。
    /// </summary>
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    /// <summary>
    /// kGpuBufferType_SRVを返します。
    /// </summary>
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_SRV;
    }
    /// <summary>
    /// リソースが作成済みかどうかを取得します。
    /// </summary>
    bool IsValid() override;
    /// <summary>
    /// リソースの状態を遷移させるバリアを積みます。
    /// </summary>
    /// <param name="new_state">遷移先の状態</param>
    /// <returns>遷移を行った場合はtrue</returns>
    bool Transition(D3D12_RESOURCE_STATES new_state) override;

    /// <summary>
    /// テクスチャは読み戻しに対応していないため、警告のみ出力します。
    /// </summary>
    void RequestReadBack() override;
    /// <summary>
    /// テクスチャは読み戻しに対応していないため、警告を出力してfalseを返します。
    /// </summary>
    bool FetchBufferData(void *data) override;

    /// <summary>
    /// リソースを取得します。未作成の場合はCreateBufferで作成します。
    /// </summary>
    ID3D12Resource *Resource() override;
    /// <summary>
    /// Texture2DとしてのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

};
}