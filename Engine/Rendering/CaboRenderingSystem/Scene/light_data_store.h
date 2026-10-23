#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/Core/slot_pool.h"

namespace CRS
{
using LightId = Handle<struct LightTag>;

enum class LightType : int32_t
{
    Directional = 0,
    Spot = 1,
};

/// <summary>
/// ライトの描画に必要なデータです。
/// </summary>
struct LightData
{
    LightType type = LightType::Directional;
    Color color = Color(1, 1, 1, 1);
    float intensity = 1.0f;
    // スポットライトの届く距離
    float range = 10.0f;
    Vector3 position;
    Vector3 direction = Vector3(0, -1, 0);
    float inner_cos = 0.7f;
    float outer_cos = 0.6f;
    bool cast_shadow = false;
};

/// <summary>
/// ライトのデータを持つストアです。
/// </summary>
class LightDataStore
{
    SlotPool<LightTag, LightData> m_lights_;

public:
    LightId Register(const LightData &data);
    void Unregister(LightId id);
    void Update(LightId id, const LightData &data);
    bool IsValid(LightId id) const;
    size_t Count() const;

    std::vector<LightData> TakeSnapshot() const;
};
}
