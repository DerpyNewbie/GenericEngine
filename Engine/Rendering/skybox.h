#pragma once
#include "shader.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/TextureCube.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
/// <summary>
/// TextureCubeを使ってSkyboxを描画するクラスです。
/// </summary>
class Skybox
{
    std::shared_ptr<TextureCube> m_texture_cube_;
    std::shared_ptr<VertexBuffer> m_vertex_buffer_;
    std::shared_ptr<IndexBuffer> m_index_buffer_;
    std::shared_ptr<DescriptorHandle> m_texture_cube_handle_;
    std::shared_ptr<Shader> m_skybox_shader_;

    /// <summary>
    /// Cubeの頂点とIndexのBuffer、空のTextureCubeを作成します。
    /// </summary>
    Skybox();
    /// <summary>
    /// TextureCubeをDescriptorHeapに登録し直します。
    /// </summary>
    /// <returns>TextureCubeが無効な場合 false</returns>
    bool ReconstructTextureCube();

public:
    /// <summary>
    /// Skyboxの唯一のインスタンスを取得します。
    /// </summary>
    static std::shared_ptr<Skybox> Instance();

    /// <summary>
    /// TextureCubeが有効な場合に、Skyboxを描画します。
    /// </summary>
    void Render();

    /// <summary>
    /// 設定されているTextureCubeを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<TextureCube> TextureCube() const;

    /// <summary>
    /// Skyboxに使うTextureCubeを設定します。
    /// </summary>
    /// <param name="texture_cube">使用するTextureCube</param>
    void SetTextureCube(const std::shared_ptr<class TextureCube> &texture_cube);
};
}