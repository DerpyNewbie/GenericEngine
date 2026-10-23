#pragma once
#include <assimp/texture.h>
#include "Asset/asset_ptr.h"
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"

namespace engine
{
/// <summary>
/// 2次元のTextureのAssetです。サイズやフォーマットなどの情報を持ち、ピクセルデータは必要になった時に画像ファイルから読み込みます。
/// </summary>
class Texture2D : public Object, public Inspectable
{
    friend class Texture2DImporter;

    /// <summary>
    /// 読み込みに使用する画像ファイルの形式です。
    /// </summary>
    enum class kImageFormat : uint8_t
    {
        kUnknown = 0,
        kWic,
        kTga
    };

    /// <summary>
    /// 拡張子から画像ファイルの形式を判定します。
    /// </summary>
    /// <param name="file_path">画像ファイルのパス</param>
    /// <returns>WICで読み込める形式ならkWic、TGAならkTga、それ以外はkUnknown</returns>
    static kImageFormat GetImageFormat(const std::filesystem::path &file_path);

    /// <summary>
    /// 画像ファイルを読み込み、メタデータと画像データを取得します。WIC形式の場合は上下を反転します。
    /// </summary>
    /// <param name="file_path">画像ファイルのパス</param>
    /// <param name="metadata">読み込んだメタデータの出力先</param>
    /// <param name="scratch">読み込んだ画像データの出力先</param>
    static void LoadMetadata(const std::filesystem::path &file_path, DirectX::TexMetadata &metadata, DirectX::ScratchImage &scratch);
    /// <summary>
    /// Assetの画像ファイルを読み込み、幅、高さ、フォーマット、MipLevelをキャッシュします。Assetでない場合は何もしません。
    /// </summary>
    void CacheData();

protected:
    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_ = {};
    uint32_t m_width_ = 0;
    uint32_t m_height_ = 0;
    uint16_t m_mip_level_ = UINT16_MAX;
    DXGI_FORMAT m_format_ = DXGI_FORMAT_UNKNOWN;
    std::string m_path_;
    uint64_t m_id_ = -1;
    bool m_enable_read_write_ = false;

    // GPU上のテクスチャ。初めて使う時に作る
    std::shared_ptr<CRS::Texture> m_gpu_texture_;

    /// <summary>
    /// GPU上のテクスチャを作成します。既定ではピクセルデータ(RGBA8)を読み込んだテクスチャを作ります。
    /// </summary>
    virtual std::shared_ptr<CRS::Texture> CreateGpuTexture();
    /// <summary>
    /// 作成済みのGPU上のテクスチャが、今の設定(サイズなど)と合っているかどうかです。合っていない場合は作り直します。
    /// </summary>
    virtual bool IsGpuTextureUpToDate() const;

public:
    Texture2D() = default;
    /// <summary>
    /// サイズとフォーマットを指定してTextureを生成します。
    /// </summary>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    /// <param name="mip_level">MipLevel</param>
    /// <param name="format">フォーマット</param>
    Texture2D(uint32_t width, uint32_t height, uint16_t mip_level, DXGI_FORMAT format);

    /// <summary>
    /// Assimpの埋め込みTextureからピクセルデータを読み込みます。圧縮されている場合はstbでデコードします。
    /// </summary>
    /// <param name="ai_texture">読み込むTexture</param>
    void LoadFromAiTexture(aiTexture *ai_texture);
    
    void OnInspectorGui() override;

    /// <summary>
    /// ピクセルデータを取得します。Assetの場合は画像ファイルから読み込み直し、そうでない場合は保持しているデータを返します。
    /// </summary>
    virtual std::vector<DirectX::PackedVector::XMCOLOR> GetPixels();

    /// <summary>
    /// 幅を取得します。未取得の場合は画像ファイルから読み込みます。
    /// </summary>
    [[nodiscard]] uint32_t Width();

    /// <summary>
    /// 高さを取得します。未取得の場合は画像ファイルから読み込みます。
    /// </summary>
    [[nodiscard]] uint32_t Height();

    /// <summary>
    /// MipLevelを取得します。未取得の場合は画像ファイルから読み込みます。
    /// </summary>
    [[nodiscard]] uint16_t MipLevel();

    /// <summary>
    /// フォーマットを取得します。未取得の場合は画像ファイルから読み込みます。
    /// </summary>
    [[nodiscard]] DXGI_FORMAT Format();

    /// <summary>
    /// 描画に使うGPU上のテクスチャを取得します。初回、または設定が変わった時に作ります。
    /// </summary>
    CRS::TextureHandle GpuTexture();
    /// <summary>
    /// GPU上のテクスチャを破棄します。次にGpuTextureを呼んだ時に作り直します（画像を読み込み直した時など）。
    /// </summary>
    void ReleaseGpuTexture();

    /// <summary>
    /// 破棄時に、GPU上のテクスチャを解放します。
    /// </summary>
    void OnDestroy() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            cereal::make_nvp("width", m_width_),
            cereal::make_nvp("height", m_height_),
            cereal::make_nvp("format", m_format_),
            cereal::make_nvp("mip_level", m_mip_level_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Texture2D, 1)