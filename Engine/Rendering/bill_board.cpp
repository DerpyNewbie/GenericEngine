#include "bill_board.h"

#include "game_object.h"
#include "logger.h"
#include "update_manager.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/camera.h"

#include <directx/d3dcommon.h>
using namespace DirectX::SimpleMath;

void BillBoard::OnConstructed()
{
    engine::UpdateManager::SubscribeDrawCall(shared_from_base<BillBoard>());
}

void BillBoard::OnDraw()
{
    //create vertex buffer
    if (!m_vertex_buffer_)
    {
        std::array<engine::Vertex, 4> vertices;
        vertices[0] = {Vector3(-5, -5, 0.0f), Color(1, 1, 1, 1)};
        vertices[1] = {Vector3(-5, 5, 0.0f), Color(1, 1, 1, 1)};
        vertices[2] = {Vector3(5, -5, 0.0f), Color(1, 1, 1, 1)};
        vertices[3] = {Vector3(5, 5, 0.0f), Color(1, 1, 1, 1)};

        m_vertex_buffer_ = std::make_shared<engine::VertexBuffer>(vertices.size(), vertices.data());
        if (!m_vertex_buffer_->IsValid())
        {
            engine::Logger::Error<BillBoard>("Failed to create VertexBuffer!");
            return;
        }
    }

    //create index buffer
    if (!m_index_buffer_)
    {
        std::vector<uint32_t> indices =
        {
            0, 1, 2, 3, 1, 2
        };
        m_index_buffer_ = std::make_shared<engine::IndexBuffer>(indices.size() * sizeof(uint32_t), indices.data());
        if (!m_index_buffer_->IsValid())
        {
            engine::Logger::Error<BillBoard>("Failed to create IndexBuffer!");
            return;
        }
    }

    const auto camera = engine::Camera::Current().lock();
    if (camera == nullptr)
    {
        engine::Logger::Error<BillBoard>("Main Camera is not set!");
        return;
    }

    auto obj_pos = world_matrix.Translation();
    auto camera_transform = camera->GameObject()->Transform()->WorldMatrix();
    auto camera_pos = camera_transform.Translation();
    Vector3 dir_vec3 = camera_pos - obj_pos;
    DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(XMLoadFloat3(&dir_vec3));

    // 回転角度を算出（XZとYZで）
    float angle_y = std::atan2(dir_vec3.x, dir_vec3.z);
    float value = dir_vec3.y / DirectX::XMVectorGetX(DirectX::XMVector3Length(XMLoadFloat3(&dir_vec3)));
    value = std::clamp(value, -1.0f, 1.0f);
    float angle_x = -std::asin(value);

    // 合成（X → Y 回転順）
    auto rot_y = Matrix(DirectX::XMMatrixRotationY(angle_y));
    auto rot_x = Matrix(DirectX::XMMatrixRotationX(angle_x));
    auto trans = Matrix(DirectX::XMMatrixTranslation(obj_pos.x, obj_pos.y, obj_pos.z));

    world_matrix = rot_x * rot_y * trans;

    for (auto &wvp_buffer : m_wvp_buffers_)
    {
        if (!wvp_buffer)
        {
            wvp_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix));
            wvp_buffer->CreateBuffer();
        }
        Matrix wvp;
        wvp = world_matrix;
        wvp_buffer->UpdateBuffer(&wvp);
    }

    const auto buffer_index = g_RenderEngine->CurrentBackBufferIndex();

    const auto cmd_list = g_RenderEngine->CommandList();
    const auto vertex_buffer_view = m_vertex_buffer_->View();
    auto ibView = m_index_buffer_->View();
    auto current_buffer = g_RenderEngine->CurrentBackBufferIndex();

    cmd_list->SetPipelineState(PSOManager::Get("Basic"));
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
    cmd_list->IASetIndexBuffer(&ibView);
    cmd_list->SetGraphicsRootConstantBufferView(engine::kWVPCBV, m_wvp_buffers_[current_buffer]->GetAddress());

    cmd_list->DrawIndexedInstanced(6, 1, 0, 0, 0);
}