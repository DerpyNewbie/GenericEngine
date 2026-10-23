#pragma once
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"

namespace engine
{
/// <summary>
/// spritefontファイル（DirectXTKのMakeSpriteFontの出力）を読み込んだFontのデータです。
/// 文字ごとの位置と、文字の画像（Texture）のピクセルデータを持ちます。GPUへの登録は描画側(CrsBridge)が行います。
/// </summary>
class FontData : public Object
{
public:
    /// <summary>
    /// 1文字分の情報です。
    /// </summary>
    struct Glyph
    {
        uint32_t character = 0;
        // Texture内の範囲（ピクセル）
        int32_t left = 0;
        int32_t top = 0;
        int32_t right = 0;
        int32_t bottom = 0;
        float x_offset = 0;
        float y_offset = 0;
        float x_advance = 0;
    };

    /// <summary>
    /// 文字の画像です。formatはDXGI_FORMATの値です。
    /// </summary>
    struct TextureData
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t format = 0;
        // 1行（ブロック圧縮の場合は4ピクセル分の1行）のバイト数
        uint32_t row_pitch = 0;
        uint32_t row_count = 0;
        std::vector<uint8_t> pixels;
    };

private:
    std::vector<Glyph> m_glyphs_;
    float m_line_spacing_ = 0;
    uint32_t m_default_character_ = 0;
    TextureData m_texture_;
    // GPU上の文字の画像。初めて使う時に作る
    std::shared_ptr<CRS::Texture> m_gpu_texture_;

public:
    /// <summary>
    /// 描画に使うGPU上の文字の画像を取得します。初回に作ります。
    /// </summary>
    CRS::TextureHandle GpuTexture();

    /// <summary>
    /// spritefontファイルを読み込みます。
    /// </summary>
    /// <param name="font_path">spritefontファイルのパス</param>
    /// <returns>読み込めた場合はtrue</returns>
    bool LoadFont(const std::wstring &font_path);

    /// <summary>
    /// 文字の情報を取得します。無い場合は既定の文字、それも無い場合はnullptrを返します。
    /// </summary>
    const Glyph *FindGlyph(uint32_t character) const;

    float LineSpacing() const
    {
        return m_line_spacing_;
    }

    const TextureData &Texture() const
    {
        return m_texture_;
    }

    /// <summary>
    /// 文字列の各文字を、左上を原点とした位置(ピクセル)とともに順に渡します。改行に対応します。
    /// </summary>
    /// <param name="string">文字列（ASCII）</param>
    /// <param name="action">(文字の情報, 描く位置)を受け取る関数</param>
    void ForEachGlyph(const std::string &string, const std::function<void(const Glyph &, Vector2)> &action) const;
};
}
