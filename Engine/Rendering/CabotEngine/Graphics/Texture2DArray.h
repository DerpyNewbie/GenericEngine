#pragma once
#include "Texture2D.h"
#include "Asset/asset_ptr.h"
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

/// <summary>
/// 複数のTexture2Dを1つにまとめたTextureの配列です。
/// </summary>
class Texture2DArray : public engine::ShaderResource
{
    std::vector<engine::AssetPtr<Texture2D>> m_textures_;
    ComPtr<ID3D12Resource> m_buffer_;
    DXGI_FORMAT m_format_ = {};
    uint32_t m_mip_level_ = 1;
    uint16_t m_element_count_ = 0;
    bool m_is_valid_ = false;

    /// <summary>
    /// 登録されているTextureの数に合わせてResourceを作り直し、各Textureの内容をコピーします。
    /// </summary>
    void CopyResource();

public:
    /// <summary>
    /// Texture2DArrayのResourceを作成します。
    /// </summary>
    /// <param name="size">1枚のサイズ</param>
    /// <param name="elem_count">枚数</param>
    /// <param name="mip_level">MipLevelの数</param>
    /// <param name="format">Format</param>
    /// <param name="flags">Resourceのflag</param>
    /// <param name="clear_value">クリアする時の値</param>
    /// <returns>作成に失敗した場合 false</returns>
    bool CreateResource(Vector2 size, uint16_t elem_count, uint16_t mip_level = 1,
                        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
                        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
                        D3D12_CLEAR_VALUE *clear_value = nullptr);
    /// <summary>
    /// ShaderResourceViewをDescriptorHeapに登録します。
    /// </summary>
    /// <returns>登録されたDescriptorHandle</returns>
    std::shared_ptr<DescriptorHandle> UploadBuffer();
    /// <summary>
    /// Resourceが有効であるかどうかを取得します。
    /// </summary>
    bool IsValid() const;

    /// <summary>
    /// Texture2DArrayのResourceを取得します。
    /// </summary>
    ID3D12Resource *Resource() override;
    /// <summary>
    /// Texture2DArrayとして読むためのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    /// <summary>
    /// Textureを追加し、Resourceを作り直します。
    /// </summary>
    /// <param name="texture">追加するTexture</param>
    void AddTexture(engine::AssetPtr<Texture2D> texture);
    /// <summary>
    /// Textureを取り除き、Resourceを作り直します。
    /// </summary>
    /// <param name="texture">取り除くTexture</param>
    void RemoveTexture(engine::AssetPtr<Texture2D> texture);

    /// <summary>
    /// SRVで使うFormatを設定します。
    /// </summary>
    /// <param name="format">Format</param>
    void SetFormat(DXGI_FORMAT format);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m_textures_);
    }
};