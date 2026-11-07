#pragma once
#include "rendering_constants.h"
#include "Components/light.h"

namespace engine
{
/// <summary>
/// シーン内のLightと、影のカスケードの分割位置を管理するクラスです。
/// 描画（ライトのデータの転送、影の描画）はCaboRenderingSystemが行います。
/// </summary>
class Lighting
{
    std::vector<std::shared_ptr<Light>> m_lights_;
    std::array<float, RenderingConstants::kShadowCascadeCount> m_cascade_slices_ = {10.0f, 30.0f, 100.0f};

public:
    /// <summary>
    /// Lightingの唯一のインスタンスを取得します。
    /// </summary>
    static Lighting *Instance();

    /// <summary>
    /// 登録されているLightの数を取得します。
    /// </summary>
    uint32_t GetLightCount() const;
    /// <summary>
    /// 登録されているLightを取得します。
    /// </summary>
    const std::vector<std::shared_ptr<Light>> &Lights() const
    {
        return m_lights_;
    }

    /// <summary>
    /// Lightを登録します。
    /// </summary>
    /// <param name="light">登録するLight</param>
    void AddLight(std::shared_ptr<Light> light);
    /// <summary>
    /// Lightの登録を解除します。
    /// </summary>
    /// <param name="light">解除するLight</param>
    void RemoveLight(const std::shared_ptr<Light> &light);

    /// <summary>
    /// 影のカスケードを切り替える、カメラからの距離を設定します。
    /// </summary>
    /// <param name="shadow_cascade_slices">各カスケードの境界の距離</param>
    void SetCascadeSlices(const std::array<float, RenderingConstants::kShadowCascadeCount> &shadow_cascade_slices);
    /// <summary>
    /// 影のカスケードを切り替える、カメラからの距離を取得します。
    /// </summary>
    const std::array<float, RenderingConstants::kShadowCascadeCount> &CascadeSlices() const
    {
        return m_cascade_slices_;
    }
};
}
