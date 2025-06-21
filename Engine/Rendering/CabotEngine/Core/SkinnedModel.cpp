#include "pch.h"

#include "SkinnedModel.h"
#include "Animator.h"
#include "AssimpLoader.h"
#include "../../application.h"
#include "../Graphics/DescriptorHeapManager.h"
#include "../Graphics/PSOManager.h"

using namespace DirectX;

SkinnedModel::SkinnedModel()
{}

engine::Mesh Convert(CabotMesh mesh)
{
    engine::Mesh result;
    const auto verts_size = mesh.Position.size();
    result.vertices.reserve(verts_size);
    result.colors.reserve(verts_size);
    result.uvs[0].reserve(verts_size);
    result.tangents.reserve(verts_size);
    for (int i = 0; i < verts_size; i++)
    {
        result.vertices.emplace_back(mesh.Position[i]);
        result.colors.emplace_back(mesh.Color[i].x, mesh.Color[i].y, mesh.Color[i].z, mesh.Color[i].w);
        result.uvs[0].emplace_back(mesh.UV[i]);
        result.tangents.emplace_back(mesh.Tangent[i].x, mesh.Tangent[i].y, mesh.Tangent[i].z, 1.0F);
        result.normals.emplace_back(mesh.Normal[i]);
        for (int j = 0; j < 4; j++)
        {
            std::vector<engine::BoneWeight> buff;
            for (int k = 0; k < 4; k++)
            {
                buff.emplace_back(mesh.BoneIDs[j].at(i), mesh.BoneWeights[j].at(i));
            }

            result.bone_weights.emplace_back(buff);
        }
    }

    return result;
}

bool SkinnedModel::LoadModel(std::wstring model_path)
{
    m_Materials.resize(1);
    auto texture = m_Materials[0].Textures();

    ModelResource model_resource = {
        model_path.c_str(),
        m_Meshes,
        m_Bone,
        m_Materials,
        false,
        true
    };
    AssimpLoader loader;
    if (!loader.ImportModel(model_resource))
    {
        return false;
    }

    auto &meshes = m_Meshes;

    m_VertexBuffers.reserve(meshes.size());
    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto size = sizeof(CabotVertex) * meshes[i].Position.size();
        auto stride = sizeof(CabotVertex);

        auto converted_mesh = Convert(meshes[i]);
        auto pVB = std::make_shared<engine::VertexBuffer>(&converted_mesh);
        if (!pVB->IsValid())
        {
            printf("頂点バッファの生成に失敗\n");
            return false;
        }

        m_VertexBuffers.push_back(pVB);
    }

    m_IndexBuffers.reserve(meshes.size());
    for (size_t i = 0; i < meshes.size(); i++)
    {
        /*auto size = sizeof(uint32_t) * meshes[i].Indices.size();
        auto indices = meshes[i].Indices.data();
        auto pIB = std::make_shared<IndexBuffer>(size, indices);
        if (!pIB->IsValid())
        {
            printf("インデックスバッファの生成に失敗\n");
            return false;
        }
        m_IndexBuffers.push_back(pIB);*/
    }

    m_MaterialHandles.clear();
    m_MaterialBuffers.resize(m_Materials.size());
    for (size_t i = 0; i < m_Materials.size(); i++)
    {
        m_MaterialBuffers[i].Initialize(m_Materials[i]);
        m_MaterialHandles.push_back(m_MaterialBuffers[i].Handle);
    }

    auto &armature = m_Bone.Transforms;

    m_BoneBuffer.Initialize(armature.size());
    m_BoneBuffer.Upload(armature);
    m_BoneHandle = g_DescriptorHeapManager->Get().Register(m_BoneBuffer);

    auto eyePos = XMVectorSet(0.0f, 120.0, 275.0, 0.0f);
    auto targetPos = XMVectorSet(0.0f, 120.0, 0.0, 0.0f);
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    auto fov = XMConvertToRadians(60);
    auto aspect = static_cast<float>(Application::WindowWidth()) / static_cast<float>(
                      Application::WindowHeight()); // アスペクト比

    for (size_t i = 0; i < RenderEngine::FRAME_BUFFER_COUNT; i++)
    {
        m_WVPBuffer[i] = std::make_shared<ConstantBuffer>(sizeof(Transform));
        if (!m_WVPBuffer[i]->IsValid())
        {
            printf("変換行列用定数バッファの生成に失敗\n");
            return false;
        }

        // 変換行列の登録
        auto ptr = m_WVPBuffer[i]->GetPtr<Transform>();
        ptr->World = XMMatrixRotationY(0);
        ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
        ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
    }

    return true;
}

void SkinnedModel::SetTransform(const XMMATRIX &transform) const
{
    for (size_t i = 0; i < RenderEngine::FRAME_BUFFER_COUNT; i++)
    {
        // 変換行列の登録
        auto ptr = m_WVPBuffer[i]->GetPtr<Transform>();
        ptr->World = transform;
    }
}

void SkinnedModel::ApplyAnimation()
{
    auto &armature = m_Bone.Transforms;
    m_BoneBuffer.Upload(armature);
}

void SkinnedModel::Draw()
{
    auto currentIndex = g_RenderEngine->CurrentBackBufferIndex();
    auto commandList = g_RenderEngine->CommandList();

    commandList->SetPipelineState(g_PSOManager.Get("default"));
    commandList->SetGraphicsRootConstantBufferView(0, m_WVPBuffer[currentIndex]->GetAddress());

    for (size_t i = 0; i < m_Meshes.size(); i++)
    {
        auto vbView = m_VertexBuffers[i]->View();
        auto ibView = m_IndexBuffers[i]->View();

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetVertexBuffers(0, 1, &vbView);
        commandList->IASetIndexBuffer(&ibView);

        commandList->SetGraphicsRootDescriptorTable(6, m_MaterialHandles[i]->HandleGPU); // そのメッシュに対応するディスクリプタテーブルをセット

        commandList->DrawIndexedInstanced(m_Meshes[i].Indices.size(), 1, 0, 0, 0);
    }
}