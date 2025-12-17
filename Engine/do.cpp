#include "pch.h"
#include "do.h"
#include "Math/trs.h"

namespace engine
{
Task Do::Transformation(const std::shared_ptr<Transform> &transform, const Matrix &to, float duration)
{
    auto from_trs = TRS(transform->LocalMatrix());
    auto to_trs = TRS(to * transform->WorldToLocal());
    auto start_time = Time::Get()->TimeSinceStartUp();

    while (Time::Get()->TimeSinceStartUp() <= start_time + duration)
    {
        transform->SetLocalMatrix(TRS::Blend(from_trs, to_trs, (Time::Get()->TimeSinceStartUp() - start_time) / duration).GetMatrix());
        co_await WaitForNextFrame();
    }
}

Task Do::Move(std::shared_ptr<Transform> transform, const Vector3 &to, float duration)
{
    auto from_pos = transform->Position();
    auto to_pos = to;
    auto start_time = Time::Get()->TimeSinceStartUp();

    while (Time::Get()->TimeSinceStartUp() <= start_time + duration)
    {
        transform->SetPosition(from_pos + (to_pos - from_pos) * (Time::Get()->TimeSinceStartUp() - start_time) / duration);
        co_await WaitForNextFrame();
    }

    transform->SetPosition(to_pos);
}

Task Do::Rotation(std::shared_ptr<Transform> transform, const Quaternion &to, float duration)
{
    auto from_rot = transform->Rotation();
    auto to_rot = to;
    auto start_time = Time::Get()->TimeSinceStartUp();
    auto end_time = start_time + duration;

    while (Time::Get()->TimeSinceStartUp() <= end_time)
    {
        transform->SetRotation(Mathf::Slerp(from_rot, to_rot, (Time::Get()->TimeSinceStartUp() - start_time) / duration));
        co_await WaitForNextFrame();
    }

    transform->SetRotation(to_rot);
}

Task Do::Scale(std::shared_ptr<Transform> transform, const Vector3 &to, float duration)
{
    const Vector3 from_scale = transform->Scale();

    std::shared_ptr<Transform> parent = transform->Parent();
    const Vector3 parent_scale = parent ? parent->Scale() : Vector3(1, 1, 1);

    const float start_time = Time::Get()->TimeSinceStartUp();
    const float end_time = start_time + duration;

    while (Time::Get()->TimeSinceStartUp() <= end_time)
    {
        Vector3 world = from_scale + (to - from_scale) * ((Time::Get()->TimeSinceStartUp() - start_time) / duration);

        Vector3 local(
            world.x / parent_scale.x,
            world.y / parent_scale.y,
            world.z / parent_scale.z
        );

        transform->SetLocalScale(local);
        co_await WaitForNextFrame();
    }

    Vector3 finalLocal(
        to.x / parent_scale.x,
        to.y / parent_scale.y,
        to.z / parent_scale.z
    );
    transform->SetLocalScale(finalLocal);
}
}