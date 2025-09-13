#pragma once
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/TextureCube.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class Skybox : public Component
{
    bool m_is_texture_set_ = false;

    TextureCube m_texture_cube_;
    std::shared_ptr<VertexBuffer> m_vertex_buffer_;
    std::shared_ptr<IndexBuffer> m_index_buffer_;
    std::shared_ptr<DescriptorHandle> m_texture_cube_handle_;

    bool ReconstructTextureCube();

public:
    std::array<AssetPtr<Texture2D>, 6> textures;

    static Skybox *Instance();
    static void Initialize();
    void OnInspectorGui() override;
    static void Render();
};
}