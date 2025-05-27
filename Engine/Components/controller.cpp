#include "controller.h"
#include "game_object.h"
#include "transform.h"

namespace engine
{
void Controller::OnUpdate()
{
    Component::OnUpdate();

    // take inputs
    VECTOR dir = {0, 0, 0};
    if (CheckHitKey(KEY_INPUT_W))
        dir = dir + VECTOR{0, 0, 1};
    if (CheckHitKey(KEY_INPUT_S))
        dir = dir + VECTOR{0, 0, -1};
    if (CheckHitKey(KEY_INPUT_D))
        dir = dir + VECTOR{1, 0, 0};
    if (CheckHitKey(KEY_INPUT_A))
        dir = dir + VECTOR{-1, 0, 0};
    if (CheckHitKey(KEY_INPUT_SPACE))
        dir = dir + VECTOR{0, 1, 0};
    if (CheckHitKey(KEY_INPUT_LCONTROL))
        dir = dir + VECTOR{0, -1, 0};

    VECTOR delta_rot = {0, 0, 0};
    if (CheckHitKey(KEY_INPUT_UP))
        delta_rot = delta_rot + VECTOR{-1, 0, 0};
    if (CheckHitKey(KEY_INPUT_DOWN))
        delta_rot = delta_rot + VECTOR{1, 0, 0};
    if (CheckHitKey(KEY_INPUT_LEFT))
        delta_rot = delta_rot + VECTOR{0, -1, 0};
    if (CheckHitKey(KEY_INPUT_RIGHT))
        delta_rot = delta_rot + VECTOR{0, 1, 0};
    delta_rot = delta_rot * 0.01F;
    m_rotation_ = m_rotation_ + delta_rot;

    // calculate next transform
    const auto transform = GameObject()->Transform();

    MATRIX rot_mat;
    CreateRotationXYZMatrix(&rot_mat, m_rotation_.x, m_rotation_.y, 0);
    transform->SetPosition(transform->Position() + dir * transform->Rotation());
    transform->SetRotation(rot_mat);
}
}