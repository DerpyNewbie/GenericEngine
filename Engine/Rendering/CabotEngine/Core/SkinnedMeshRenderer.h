#pragma once
#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/SharedStruct.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/IndexBuffer.h"
#include "../Graphics/RenderEngine.h"

class SkinnedMeshRenderer
{
public:
    SkinnedMeshRenderer();

    void LoadFromFBX(std::wstring file_path);
    void Draw();

private:
    std::vector<CabotMesh> m_Meshes;
    std::vector<Texture2D> m_Textures;
    std::vector<std::shared_ptr<engine::VertexBuffer>> m_VertexBuffers;
    std::vector<std::shared_ptr<IndexBuffer>> m_IndexBuffers;
    std::vector<DescriptorHandle> m_MaterialHandle;
    ConstantBuffer m_WVPBuffer[RenderEngine::FRAME_BUFFER_COUNT];
};