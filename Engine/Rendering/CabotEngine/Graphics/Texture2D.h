#pragma once
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class Texture2DImporter;
}

struct aiTexture;
class DescriptorHeap;
class DescriptorHandle;

/// <summary>
/// 2DのTextureのAssetです。
/// </summary>
class Texture2D : public engine::Object, public engine::Inspectable, public IBuffer, public engine::ShaderResource
{
    friend class engine::Texture2DImporter;

protected:
    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_;
    uint32_t m_width_ = 0;
    uint32_t m_height_ = 0;
    uint16_t m_mip_level_;
    DXGI_FORMAT m_format_;

    ComPtr<ID3D12Resource> m_buffer_ = nullptr;

public:
    void OnInspectorGui() override;
    /// <summary>
    /// 保持しているピクセルのデータからTextureのResourceを作成します。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// Texture2Dは更新に対応していません。エラーをログに出力します。
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
    /// Assimpの埋め込みTextureからピクセルのデータを読み込みます。圧縮されている場合はデコードします。
    /// </summary>
    /// <param name="ai_texture">読み込むTexture</param>
    void LoadFromAiTexture(const aiTexture *ai_texture);

    /// <summary>
    /// TextureのResourceを取得します。作成されていない場合は作成します。
    /// </summary>
    ID3D12Resource *Resource() override;
    /// <summary>
    /// Texture2Dとして読むためのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    /// <summary>
    /// ピクセルのデータのコピーを取得します。
    /// </summary>
    std::vector<DirectX::PackedVector::XMCOLOR> GetTexData()
    {
        return m_tex_data_;
    }

    /// <summary>
    /// ピクセルのデータを設定します。
    /// </summary>
    /// <param name="resource">ピクセルのデータ</param>
    void SetTexData(const std::vector<DirectX::PackedVector::XMCOLOR> &resource)
    {
        m_tex_data_ = resource;
    }

    /// <summary>
    /// 幅(pixel)を取得します。
    /// </summary>
    [[nodiscard]] uint32_t Width() const
    {
        return m_width_;
    }

    /// <summary>
    /// 高さ(pixel)を取得します。
    /// </summary>
    [[nodiscard]] uint32_t Height() const
    {
        return m_height_;
    }

    /// <summary>
    /// MipLevelの数を取得します。
    /// </summary>
    [[nodiscard]] uint16_t MipLevel() const
    {
        return m_mip_level_;
    }

    /// <summary>
    /// ピクセルのFormatを取得します。
    /// </summary>
    [[nodiscard]] DXGI_FORMAT Format() const
    {
        return m_format_;
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            cereal::make_nvp("tex_data", m_tex_data_),
            cereal::make_nvp("width", m_width_),
            cereal::make_nvp("height", m_height_),
            cereal::make_nvp("format", m_format_),
            cereal::make_nvp("mip_level", m_mip_level_)
        );
    }
};

CEREAL_CLASS_VERSION(Texture2D, 1)