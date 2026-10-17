#pragma once
#include "Texture2D.h"
#include "Rendering/buffer_base.h"
#include "Asset/asset_ptr.h"
#include "Rendering/shader_resource.h"

namespace engine
{
/// <summary>
/// 6枚のTexture2DからCubeMapを作成するTextureです。
/// </summary>
class TextureCube final : public Object, public BufferBase, public ShaderResource
{
    std::array<AssetPtr<Texture2D>, 6> m_textures_;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource_;
    D3D12_RESOURCE_STATES m_current_state_;
    
public:
    /// <summary>
    /// 6面分のTextureを指定してTextureCubeを生成します。リソースはCreateBufferで作成されます。
    /// </summary>
    /// <param name="textures">Right、Left、Bottom、Top、Front、Backの順のTexture</param>
    TextureCube(const std::array<AssetPtr<Texture2D>, 6> &textures = {});
    /// <summary>
    /// リソースをDirectXResourceFactoryの解放待ちに登録し、GPUの使用が終わってから解放されるようにします。
    /// </summary>
    ~TextureCube() override;

    /// <summary>
    /// 6枚のTextureがすべて有効で同じサイズであることを確認し、CubeMapのリソースを作成して各面にコピーします。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// 更新には対応していないため、エラーを出力します。
    /// </summary>
    void UpdateBuffer(const void *data) override;
    /// <summary>
    /// 指定したDescriptorHandleの位置にTextureCubeのSRVを作成します。is_uavは無視されます。
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
    /// 読み戻しに対応していないため、警告のみ出力します。
    /// </summary>
    void RequestReadBack() override;
    /// <summary>
    /// 読み戻しに対応していないため、警告を出力してfalseを返します。
    /// </summary>
    bool FetchBufferData(void *data) override;

    /// <summary>
    /// リソースを取得します。未作成の場合はCreateBufferで作成します。
    /// </summary>
    ID3D12Resource *Resource() override;
    D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc();
    /// <summary>
    /// TextureCubeとしてのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    /// <summary>
    /// 6面分のTextureを設定し、リソースを作り直します。
    /// </summary>
    /// <param name="textures">Right、Left、Bottom、Top、Front、Backの順のTexture</param>
    /// <returns>リソースの作成に成功した場合はtrue</returns>
    bool SetTextures(const std::array<AssetPtr<Texture2D>, 6> &textures);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(m_textures_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::TextureCube, 1)