#include "pch.h"

#include "billboard_renderer.h"
#include "game_object.h"
#include "Asset/asset_database.h"
#include "Rendering/primitives.h"
#include "Rendering/render_pipeline.h"
#include "Components/camera_component.h"

namespace
{
Matrix LookAtCamera(Matrix &mat)
{
    const auto camera = engine::CameraComponent::Main();
    if (camera == nullptr)
    {
        return Matrix::Identity;
    }

    Vector3 pos;
    Quaternion rot;
    Vector3 scale;

    mat.Decompose(scale, rot, pos);
    auto camera_transform = camera->GameObject()->Transform()->WorldMatrix();
    auto camera_pos = camera_transform.Translation();
    Vector3 dir_vec3 = camera_pos - pos;

    float angle_y = std::atan2(dir_vec3.x, dir_vec3.z);
    float value = dir_vec3.y / DirectX::XMVectorGetX(DirectX::XMVector3Length(XMLoadFloat3(&dir_vec3)));
    value = std::clamp(value, -1.0f, 1.0f);
    float angle_x = -std::asin(value);

    auto rot_y = Matrix(DirectX::XMMatrixRotationY(angle_y));
    auto rot_x = Matrix(DirectX::XMMatrixRotationX(angle_x));
    auto scale_mat = Matrix(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z));
    auto trans = Matrix(DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));

    return scale_mat * rot_x * rot_y * trans;
}
}

namespace engine
{

void BillboardRenderer::UpdateWorldBuffer()
{
    for (auto &world_matrix_buffer : m_world_matrix_buffers_)
    {
        if (!world_matrix_buffer)
        {
            world_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
            world_matrix_buffer->CreateBuffer();
        }
    }

    auto world_matrix = GameObject()->Transform()->WorldMatrix();

    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto &world_matrix_buffer = m_world_matrix_buffers_[current_buffer_idx];
    const auto ptr = world_matrix_buffer->GetPtr<Matrix>();
    *ptr = LookAtCamera(world_matrix);
}

void BillboardRenderer::OnConstructed()
{
    shared_materials.emplace_back(AssetPtr<Material>::FromInstance(Instantiate<Material>()));
    const auto asset_ptr = AssetDatabase::GetAsset("BillboardShader.hlsl");
    shared_materials[0].CastedLock()->shader = AssetPtr<Shader>::FromIAssetPtr(asset_ptr);
    DirectX::BoundingBox::CreateFromPoints(bounds, Vector3(0, 0, 0), Vector3(1, 1, 1));
}

void BillboardRenderer::OnInspectorGui()
{
    Renderer::OnInspectorGui();

    for (int i = 0; i < shared_materials.size(); ++i)
    {
        ImGui::PushID(i);
        shared_materials[i].CastedLock()->OnInspectorGui();
        ImGui::PopID();
    }
}

void BillboardRenderer::Render()
{
    UpdateWorldBuffer();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();

    RenderPipeline::Submit(Primitives::GetQuadMesh(), shared_materials, GameObject()->Transform()->Position(), m_world_matrix_buffers_[current_buffer_idx]->GetAddress());
}

Matrix BillboardRenderer::BoundsOrigin()
{
    return GameObject()->Transform()->WorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::BillboardRenderer)