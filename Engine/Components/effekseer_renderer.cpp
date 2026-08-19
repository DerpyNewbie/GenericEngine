#include "pch.h"
#include "Rendering/Effect/effekseer_controller.h"
#include "effekseer_renderer.h"
#include "gui.h"

void engine::EffekseerRenderer::OnInspectorGui()
{
    Gui::PropertyField("Effekseer Effect", m_effect_asset_);
    Gui::PropertyField("Play Speed", play_speed);
    if (ImGui::Button("Play"))
    {
        Play();
    }
}

void engine::EffekseerRenderer::Render()
{
    const auto effect = m_effect_asset_.CastedLock();
    if (effect == nullptr)
        return;

    if (m_effect_ == nullptr)
    {
        const std::string path_str = effect->path;
        char16_t utf16_path[512];
        Effekseer::ConvertUtf8ToUtf16(utf16_path, 512, path_str.c_str());

        m_effect_ = Effekseer::Effect::Create(EffekseerController::Manager(), utf16_path);
    }
}

Matrix engine::EffekseerRenderer::BoundsOrigin()
{
    return GameObject()->Transform()->WorldMatrix();
}

void engine::EffekseerRenderer::SetPosition(const Vector3 pos) const
{
    EffekseerController::Manager()->SetLocation(m_effect_handle_, pos.x, pos.y, pos.z);
}

void engine::EffekseerRenderer::SetRotation(const Quaternion rot) const
{
    const auto rot_vec3 = rot.ToEuler();
    EffekseerController::Manager()->SetRotation(m_effect_handle_, rot_vec3.x, rot_vec3.y, rot_vec3.z);
}

void engine::EffekseerRenderer::SetScale(const Vector3 scale) const
{
    EffekseerController::Manager()->SetScale(m_effect_handle_, scale.x, scale.y, scale.z);
}

void engine::EffekseerRenderer::SetSpeed(const float speed) const
{
    EffekseerController::Manager()->SetSpeed(m_effect_handle_, speed);
}

void engine::EffekseerRenderer::OnUpdate()
{
    const auto transform = GameObject()->Transform();
    const auto position = transform->Position();
    const auto rotation = transform->Rotation();
    const auto scale = transform->Scale();

    SetPosition(position);
    SetRotation(rotation);
    SetScale(scale);
    SetSpeed(play_speed);
}

void engine::EffekseerRenderer::OnDestroy()
{
    EffekseerController::Manager()->StopAllEffects();
}

void engine::EffekseerRenderer::Play()
{
    const auto transform = GameObject()->Transform();
    const auto position = transform->Position();
    const auto rotation = transform->Rotation();
    const auto scale = transform->Scale();

    m_effect_handle_ = EffekseerController::Manager()->Play(m_effect_, position.x, position.y, position.z);
    SetPosition(position);
    SetRotation(rotation);
    SetScale(scale);
    SetSpeed(play_speed);
}

void engine::EffekseerRenderer::Stop() const
{
    EffekseerController::Manager()->StopEffect(m_effect_handle_);
}

CEREAL_REGISTER_TYPE(engine::EffekseerRenderer)