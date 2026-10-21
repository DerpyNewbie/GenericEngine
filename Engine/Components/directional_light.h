#pragma once
#include "light.h"
#include "Rendering/rendering_constants.h"

namespace engine
{
/// <summary>
/// 平行光源のLightです。影はカメラからの距離で3段階に分けて描きます（カスケード）。
/// </summary>
class DirectionalLight : public Light
{
public:
    /// <summary>
    /// 影のカスケードを切り替える、カメラからの距離を設定します。
    /// </summary>
    /// <param name="shadow_cascade_slices">各カスケードの境界の距離</param>
    static void SetCascadeSlices(
        const std::array<float, RenderingConstants::kShadowCascadeCount> &shadow_cascade_slices
    );

    /// <summary>
    /// Lightの種類を平行光源に設定します。
    /// </summary>
    void OnConstructed() override;
    void OnInspectorGui() override;
    /// <summary>
    /// Transformの前方向をLightの向きとして反映します。
    /// </summary>
    void UpdateData() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Light>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::DirectionalLight, 1)
