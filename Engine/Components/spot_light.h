#pragma once
#include "light.h"

namespace engine
{
/// <summary>
/// スポットライトのLightです。
/// </summary>
class SpotLight : public Light
{
public:
    /// <summary>
    /// Lightの種類をスポットライトに設定し、既定の角度(内側45度、外側50度)を設定します。
    /// </summary>
    void OnConstructed() override;
    void OnInspectorGui() override;
    /// <summary>
    /// Transformの位置と前方向をLightに反映します。
    /// </summary>
    void UpdateData() override;

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

CEREAL_CLASS_VERSION(engine::SpotLight, 1)
