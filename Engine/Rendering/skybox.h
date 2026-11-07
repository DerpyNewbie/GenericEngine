#pragma once
#include "Rendering/texture_cube.h"

namespace engine
{
/// <summary>
/// スカイボックスに使うTextureCubeを持つシングルトンクラスです。描画はCaboRenderingSystemが行います。
/// </summary>
class Skybox
{
    std::shared_ptr<TextureCube> m_texture_cube_;

    Skybox() = default;

public:
    /// <summary>
    /// Skyboxの唯一のインスタンスを取得します。
    /// </summary>
    static std::shared_ptr<Skybox> Instance();

    /// <summary>
    /// 設定されているTextureCubeを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<TextureCube> TextureCube() const;
    /// <summary>
    /// スカイボックスに使うTextureCubeを設定します。
    /// </summary>
    /// <param name="texture_cube">設定するTextureCube。nullptrの場合はスカイボックスを描きません</param>
    void SetTextureCube(const std::shared_ptr<class TextureCube> &texture_cube);
};
}
