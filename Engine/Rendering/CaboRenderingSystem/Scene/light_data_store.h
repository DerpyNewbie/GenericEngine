#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/Core/slot_pool.h"

namespace CRS
{
// LightDataStoreに登録したライトを指すID
using LightId = Handle<struct LightTag>;

/// <summary>
/// ライトの種類です。シェーダーの LIGHT_TYPE_* と同じ値です。
/// </summary>
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
    // スポットライトの位置（平行光源では使わない）
    Vector3 position;
    // 光の進む向き
    Vector3 direction = Vector3(0, -1, 0);
    // スポットライトの円錐の内側 / 外側の角度のcos。内側は明るさ100%で、外側に向けて0%まで減る
    float inner_cos = 0.7f;
    float outer_cos = 0.6f;
    // 影を落とすか。平行光源はカスケード3枚、スポットライトは1枚のシャドウマップを使う
    bool cast_shadow = false;
};

/// <summary>
/// ライトのデータを持つストアです。描画の前にスナップショットを取り、ShadowSystemがライトの配列と影を作ります。
/// </summary>
class LightDataStore
{
    SlotPool<LightTag, LightData> m_lights_;

public:
    /// <summary>
    /// ライトを登録します。
    /// </summary>
    LightId Register(const LightData &data);
    /// <summary>
    /// ライトを取り除きます。
    /// </summary>
    void Unregister(LightId id);
    /// <summary>
    /// データを差し替えます。無効なIDなら何もしません。
    /// </summary>
    void Update(LightId id, const LightData &data);
    /// <summary>
    /// まだ登録されているIDかどうかです。
    /// </summary>
    bool IsValid(LightId id) const;
    /// <summary>
    /// 登録されているライトの数です。
    /// </summary>
    size_t Count() const;

    /// <summary>
    /// 全ライトのコピーを作ります。
    /// </summary>
    std::vector<LightData> TakeSnapshot() const;
};
}
