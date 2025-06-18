#pragma once
#include "GeometricPrimitive.h"
#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/RenderEngine.h"
#include "../Graphics/IndexBuffer.h"
#include "../Graphics/Material.h"
#include "../Graphics/MaterialBuffer.h"
#include "../Graphics/SharedStruct.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/StructuredBuffer.h"

class SkinnedModel
{
public:
    SkinnedModel();
    bool LoadModel(std::wstring model_path);

    void SetTransform(const DirectX::XMMATRIX& transform) const;
    
    void ApplyAnimation();
    void Draw();

private:
    std::vector<Mesh> m_Meshes;
    Bone m_Bone;
    std::vector<Material> m_Materials;
    std::vector<MaterialBuffer> m_MaterialBuffers;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::vector<std::shared_ptr<IndexBuffer>> m_IndexBuffers;
    std::shared_ptr<ConstantBuffer> m_WVPBuffer[RenderEngine::FRAME_BUFFER_COUNT];
    std::vector<std::shared_ptr<DescriptorHandle>> m_MaterialHandles;
    StructuredBuffer<DirectX::XMMATRIX> m_BoneBuffer;
    std::shared_ptr<DescriptorHandle> m_BoneHandle;

    friend class Animator;
};
