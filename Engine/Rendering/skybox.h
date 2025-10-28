#pragma once
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/TextureCube.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class Skybox
{
    std::shared_ptr<TextureCube> m_texture_cube_;
    std::shared_ptr<VertexBuffer> m_vertex_buffer_;
    std::shared_ptr<IndexBuffer> m_index_buffer_;
    std::shared_ptr<DescriptorHandle> m_texture_cube_handle_;

    Skybox();
    bool ReconstructTextureCube();

public:
    static std::shared_ptr<Skybox> Instance();

    void Render();

    [[nodiscard]] std::shared_ptr<TextureCube> TextureCube() const;

    void SetTextureCube(const std::shared_ptr<class TextureCube> &texture_cube);
};
}