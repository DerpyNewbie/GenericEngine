#pragma once
#include "ThirdParty/Effekseer/include/Effekseer/Effekseer.h"

#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Asset/effekseer_effect.h"

namespace engine
{
class EffekseerRenderer : public Renderer
{
    AssetPtr<EffekseerEffect> m_effect_asset_;
    Effekseer::Handle m_effect_handle_ = -1;
    Effekseer::EffectRef m_effect_;

    void SetPosition(Vector3 pos) const;
    void SetRotation(Quaternion rot) const;
    void SetScale(Vector3 scale) const;
    void SetSpeed(float speed) const;

public:
    float play_speed = 1.0f;

    void Play();
    void Stop() const;

    void OnInspectorGui() override;
    void OnUpdate() override;
    void OnDestroy() override;

    void Render() override;
    Matrix BoundsOrigin() override;
    
    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Renderer>(this),
            CEREAL_NVP(m_effect_asset_),
            CEREAL_NVP(play_speed)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::EffekseerRenderer, 1)