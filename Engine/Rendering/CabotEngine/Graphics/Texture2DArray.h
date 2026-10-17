#pragma once
#include "Texture2D.h"
#include "Asset/asset_ptr.h"
#include "Rendering/buffer_base.h"
#include "Rendering/shader_resource.h"

namespace engine
{
/// <summary>
/// 複数のTexture2Dを1つのTexture2DArrayリソースにまとめて扱うクラスです。
/// </summary>
class Texture2DArray : public ShaderResource
{
    std::vector<AssetPtr<Texture2D>> m_textures_;
    ComPtr<ID3D12Resource> m_buffer_;
    DXGI_FORMAT m_format_ = {};
    uint32_t m_mip_level_ = 1;
    uint16_t m_element_count_ = 0;
    bool m_is_valid_ = false;

    /// <summary>
    /// 保持している全Textureを格納できるリソースを作り直し、各Textureの内容をコピーします。サイズとフォーマットは先頭のTextureに合わせます。
    /// </summary>
    void CopyResource();

public:
    /// <summary>
    /// Texture2DArrayのリソースを作成します。
    /// </summary>
    /// <param name="size">各要素のサイズ</param>
    /// <param name="elem_count">要素数</param>
    /// <param name="mip_level">MipLevel</param>
    /// <param name="format">フォーマット</param>
    /// <param name="flags">リソースのフラグ</param>
    /// <param name="clear_value">最適化用のクリア値</param>
    /// <returns>作成に成功した場合はtrue</returns>
    bool CreateResource(Vector2 size, uint16_t elem_count, uint16_t mip_level = 1,
                        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
                        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
                        D3D12_CLEAR_VALUE *clear_value = nullptr);
    /// <summary>
    /// DescriptorHeapにSRVを登録します。
    /// </summary>
    /// <returns>登録したハンドル</returns>
    std::shared_ptr<DescriptorHandle> UploadBuffer();
    /// <summary>
    /// リソースが作成済みで使用可能かどうかを取得します。
    /// </summary>
    bool IsValid() const;

    /// <summary>
    /// リソースを取得します。
    /// </summary>
    ID3D12Resource *Resource() override;
    /// <summary>
    /// Texture2DArrayとしてのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

    /// <summary>
    /// Textureを追加し、リソースを作り直します。
    /// </summary>
    /// <param name="texture">追加するTexture</param>
    void AddTexture(AssetPtr<Texture2D> texture);
    /// <summary>
    /// Textureを削除し、リソースを作り直します。空になった場合は無効になります。
    /// </summary>
    /// <param name="texture">削除するTexture</param>
    void RemoveTexture(AssetPtr<Texture2D> texture);

    /// <summary>
    /// フォーマットを設定します。
    /// </summary>
    /// <param name="format">フォーマット</param>
    void SetFormat(DXGI_FORMAT format);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m_textures_);
    }
};
}
