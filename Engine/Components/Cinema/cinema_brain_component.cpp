#include "pch.h"

#include "cinema_brain_component.h"
#include "gui.h"

namespace engine
{
void CinemaBrainComponent::OnInspectorGui()
{
    Gui::PropertyField("Target Camera", m_target_camera_);
    Gui::PropertyField("Cinema Cameras", m_cinema_cameras_);
}

void CinemaBrainComponent::OnUpdate()
{
    const auto target = m_target_camera_.CastedLock();
    if (target == nullptr)
    {
        return;
    }

    auto result = Matrix::Identity;
    for (auto &[camera_ptr, blend] : m_cinema_cameras_)
    {
        auto camera = camera_ptr.CastedLock();
        if (camera == nullptr)
        {
            continue;
        }

        camera->ApplyTransform();

        auto look_at = camera->GetLookAtMatrix();
        result += look_at * blend;
    }

    Vector3 pos, sca;
    Quaternion rot;
    result.Decompose(sca, rot, pos);

    target->GameObject()->Transform()->SetPositionAndRotation(pos, rot);
}
void CinemaBrainComponent::AddCamera(const std::shared_ptr<CinemaCameraComponent> &cinema_camera,
                                     float blend_coefficient)
{
    m_cinema_cameras_.emplace_back(AssetPtr<CinemaCameraComponent>::FromManaged(cinema_camera), blend_coefficient);
}
void CinemaBrainComponent::RemoveCamera(std::shared_ptr<CinemaCameraComponent> cinema_camera)
{
    std::erase_if(m_cinema_cameras_, [&](auto &pair) {
        return pair.first.CastedLock() == cinema_camera;
    });
}
}

CEREAL_REGISTER_TYPE(engine::CinemaBrainComponent)