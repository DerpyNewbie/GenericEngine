#include "pch.h"
#include "billboard.h"

#include "game_object.h"
#include "logger.h"
#include "Components/camera_component.h"
using namespace DirectX::SimpleMath;

Matrix Billboard::CalcMatrix(Matrix &mat)
{
    const auto camera = engine::CameraComponent::Main();
    if (camera == nullptr)
    {
        engine::Logger::Error<Billboard>("Main Camera is not set!");
        return Matrix::Identity;
    }

    Vector3 pos;
    Quaternion rot;
    Vector3 scale;

    mat.Decompose(scale, rot, pos);
    auto camera_transform = camera->GameObject()->Transform()->WorldMatrix();
    auto camera_pos = camera_transform.Translation();
    Vector3 dir_vec3 = camera_pos - pos;

    // calculate angle on XZ and YZ
    float angle_y = std::atan2(dir_vec3.x, dir_vec3.z);
    float value = dir_vec3.y / DirectX::XMVectorGetX(DirectX::XMVector3Length(XMLoadFloat3(&dir_vec3)));
    value = std::clamp(value, -1.0f, 1.0f);
    float angle_x = -std::asin(value);

    // merge angles in X -> Y order
    auto rot_y = Matrix(DirectX::XMMatrixRotationY(angle_y));
    auto rot_x = Matrix(DirectX::XMMatrixRotationX(angle_x));
    auto scale_mat = Matrix(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z));
    auto trans = Matrix(DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));

    return scale_mat * rot_x * rot_y * trans;
}