#pragma once
#include "Texture2D.h"
#include "Rendering/ibuffer.h"
#include "Asset/asset_ptr.h"

namespace engine
{
/// <summary>
/// 6面のTexture2DからなるCube Mapです。
/// </summary>
class TextureCube final : public Object, public Inspectable, public IBuffer, public ShaderResource
{
    std::array<AssetPtr<Texture2D>, 6> m_textures_;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer_;

public:
    /// <summary>
    /// 6面のTextureの設定をInspectorに表示し、変更された場合はBufferを作り直します。
    /// </summary>
    void OnInspectorGui() override;
    /// <summary>
    /// 6枚のTextureからCubeMapのResourceを作成します。Textureが足りない場合や、サイズが揃っていない場合は作成されません。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// TextureCubeは更新に対応していません。エラーをログに出力します。
    /// </summary>
    void UpdateBuffer(void *data) override;
    /// <summary>
    /// ShaderResourceViewをDescriptorHeapに登録します。
    /// </summary>
    /// <returns>登録されたDescriptorHandle</returns>
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    /// <summary>
    /// 常に false を返します。
    /// </summary>
    bool CanUpdate() override;
    /// <summary>
    /// Resourceが作成済みであるかどうかを取得します。
    /// </summary>
    bool IsValid() override;

    /// <summary>
    /// CubeMapのResourceを取得します。作成されていない場合は作成します。
    /// </summary>
    ID3D12Resource *Resource() override;
    /// <summary>
    /// TextureCubeとして読むためのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    /// <summary>
    /// 6面のTextureを設定し、Resourceを作り直します。
    /// </summary>
    /// <param name="textures">Right、Left、Top、Bottom、Front、Backの順のTexture</param>
    /// <returns>Resourceの作成に成功した場合 true</returns>
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