#pragma once
#include "pch.h"
#include "component.h"
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"
#include "Rendering/CabotEngine/Graphics/TextureCube.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class SkyBox : public Renderer
{
    bool m_IsTextureSet = false;;

    TextureCube m_TextureCube_;
    std::shared_ptr<VertexBuffer> m_VertexBuffer_;
    std::shared_ptr<IndexBuffer> m_IndexBuffer_;
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::FRAME_BUFFER_COUNT> viewproj_buffers;
    std::shared_ptr<DescriptorHandle> m_TextureCubeHandle_;

public:
    std::array<AssetPtr<Texture2D>, 6> textures;

    void OnConstructed() override;
    void OnInspectorGui() override;
    void OnDraw() override;

    std::shared_ptr<Transform> BoundsOrigin() override;
};
}