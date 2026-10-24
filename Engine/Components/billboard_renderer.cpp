#include "pch.h"

#include "billboard_renderer.h"
#include "game_object.h"
#include "Asset/asset_database.h"
#include "Rendering/crs_bridge.h"
#include "Rendering/primitives.h"
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
    float value = dir_vec3.y / dir_vec3.Length();
    value = std::clamp(value, -1.0f, 1.0f);
    float angle_x = -std::asin(value);

    auto rot_y = Matrix::CreateRotationY(angle_y);
    auto rot_x = Matrix::CreateRotationX(angle_x);
    auto scale_mat = Matrix::CreateScale(scale.x, scale.y, scale.z);
    auto trans = Matrix::CreateTranslation(pos.x, pos.y, pos.z);

    return scale_mat * rot_x * rot_y * trans;
}
}

namespace engine
{

void BillboardRenderer::OnVisibilityChanged(const bool visible)
{
    if (!visible)
        CrsBridge::RemoveMeshRenderer(m_crs_id_);
}

void BillboardRenderer::OnConstructed()
{
    shared_materials.emplace_back(AssetPtr<Material>::FromInstance(Instantiate<Material>()));
    const auto asset_ptr = AssetDatabase::GetAsset("Unlit.hlsl");
    shared_materials[0]->SetShader(AssetPtr<Shader>::FromIAssetPtr(asset_ptr));
    DirectX::BoundingBox::CreateFromPoints(bounds, Vector3(0, 0, 0), Vector3(1, 1, 1));
}

void BillboardRenderer::OnInspectorGui()
{
    Renderer::OnInspectorGui();

    for (int i = 0; i < shared_materials.size(); ++i)
    {
        ImGui::PushID(i);
        shared_materials[i]->OnInspectorGui();
        ImGui::PopID();
    }
}

void BillboardRenderer::Render()
{
    auto world_matrix = GameObject()->Transform()->WorldMatrix();
    CrsBridge::SubmitMeshRenderer(m_crs_id_, Primitives::GetQuadMesh(), shared_materials, LookAtCamera(world_matrix),
                                  bounds);
}

Matrix BillboardRenderer::BoundsOrigin()
{
    return GameObject()->Transform()->WorldMatrix();
}
}

CEREAL_REGISTER_TYPE(engine::BillboardRenderer)