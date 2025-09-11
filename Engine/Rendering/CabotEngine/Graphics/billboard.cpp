#include "pch.h"
#include "billboard.h"

#include "game_object.h"
#include "logger.h"
#include "Components/camera_component.h"
#include "Rendering/view_projection.h"
using namespace DirectX::SimpleMath;

void Billboard::Update()
{
    // create vertex buffer
    if (!vertex_buffer)
    {
        std::array<engine::Vertex, 4> vertices;
        vertices[0].vertex = Vector3(-5, -5, 0.0f);
        vertices[1].vertex = Vector3(-5, 5, 0.0f);
        vertices[2].vertex = Vector3(5, -5, 0.0f);
        vertices[3].vertex = Vector3(5, 5, 0.0f);

        vertices[0].uvs[0] = Vector2(0, 0);
        vertices[1].uvs[0] = Vector2(0, 1);
        vertices[2].uvs[0] = Vector2(1, 0);
        vertices[3].uvs[0] = Vector2(1, 1);

        vertex_buffer = std::make_shared<engine::VertexBuffer>(vertices.size(), vertices.data());
        if (!vertex_buffer->IsValid())
        {
            engine::Logger::Error<Billboard>("Failed to create VertexBuffer!");
            return;
        }
    }

    // create index buffer
    if (!index_buffer)
    {
        std::vector<uint32_t> indices =
        {
            0, 1, 2, 3, 1, 2
        };
        index_buffer = std::make_shared<engine::IndexBuffer>(indices.size() * sizeof(uint32_t), indices.data());
        if (!index_buffer->IsValid())
        {
            engine::Logger::Error<Billboard>("Failed to create IndexBuffer!");
            return;
        }
    }

    const auto camera = engine::Camera::Main();
    if (camera == nullptr)
    {
        engine::Logger::Error<Billboard>("Main Camera is not set!");
        return;
    }

    auto obj_pos = world_matrix.Translation();
    auto camera_transform = camera->GetTransform()->WorldMatrix();
    auto camera_pos = camera_transform.Translation();
    Vector3 dir_vec3 = camera_pos - obj_pos;

    // store scale
    Vector3 scale;
    DirectX::XMMATRIX world = world_matrix;
    scale.x = DirectX::XMVectorGetX(DirectX::XMVector3Length(world.r[0]));
    scale.y = DirectX::XMVectorGetX(DirectX::XMVector3Length(world.r[1]));
    scale.z = DirectX::XMVectorGetX(DirectX::XMVector3Length(world.r[2]));

    // calculate angle on XZ and YZ
    float angle_y = std::atan2(dir_vec3.x, dir_vec3.z);
    float value = dir_vec3.y / DirectX::XMVectorGetX(DirectX::XMVector3Length(XMLoadFloat3(&dir_vec3)));
    value = std::clamp(value, -1.0f, 1.0f);
    float angle_x = -std::asin(value);

    // merge angles in X -> Y order
    auto rot_y = Matrix(DirectX::XMMatrixRotationY(angle_y));
    auto rot_x = Matrix(DirectX::XMMatrixRotationX(angle_x));
    auto scale_mat = Matrix(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z));
    auto trans = Matrix(DirectX::XMMatrixTranslation(obj_pos.x, obj_pos.y, obj_pos.z));

    world_matrix = scale_mat * rot_x * rot_y * trans;

    for (auto &wvp_buffer : wvp_buffers)
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
}