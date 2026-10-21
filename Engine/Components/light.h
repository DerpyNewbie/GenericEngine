#pragma once
#include "component.h"
#include "Rendering/light_data.h"

namespace engine
{
/// <summary>
/// Lightの種類です。
/// </summary>
enum class kLightType : uint8_t
{
    kDirectional,
    kSpotLight,
};

/// <summary>
/// すべてのLightの基底クラスです。
/// </summary>
class Light : public Component
{
protected:
    LightData m_light_data_;

public:
    void OnInspectorGui() override;
    /// <summary>
    /// Lightingに自身を登録します。
    /// </summary>
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    /// <summary>
    /// Transformの状態をLightDataに反映します。
    /// </summary>
    virtual void UpdateData() = 0;

    /// <summary>
    /// 描画に使うLightのデータを取得します。影の割り当てなどはCaboRenderingSystemが行います。
    /// </summary>
    const LightData &Data() const
    {
        return m_light_data_;
    }

    /// <summary>
    /// 影を落とすかどうかを設定します。
    /// </summary>
    void SetCastShadow(const bool cast_shadow)
    {
        m_light_data_.cast_shadow = cast_shadow ? 1 : 0;
    }

    /// <summary>
    /// Lightの色を設定します。
    /// </summary>
    void SetColor(const Color &color)
    {
        m_light_data_.color = color;
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_light_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Light, 1)