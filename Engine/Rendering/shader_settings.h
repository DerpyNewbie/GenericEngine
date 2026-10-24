#pragma once

namespace engine
{
/// <summary>
/// Shaderの描画設定(ZTest、ZWrite、Cull、Blend)です。
/// </summary>
/// <remarks>
/// .hlslファイルに次のような行を書いておくと、インポート時にその値になります（Inspectorより優先されます）。
/// <code>
/// // @ZTest LEqual   (Less / Greater / LEqual / GEqual / Equal / NotEqual / Always / Never)
/// // @ZWrite Off     (On / Off)
/// // @Cull Off       (Back / Front / Off)
/// // @Blend Alpha    (Opaque / Alpha / Additive)
/// </code>
/// </remarks>
struct ShaderSettings
{
    static constexpr std::uint32_t kSerializationVersion = 6;

    // 番号はそれぞれ ZTestNames / ZWriteNames / CullNames / BlendModeNames の並び
    int z_test = 0;
    int z_write = 0;
    int cull = 0;
    int blend_mode = 0;

    /// <summary>
    /// .hlslのソースに書かれた "// @ZTest LEqual" のような設定を読み取って反映します。
    /// </summary>
    /// <param name="source">.hlslファイルの中身</param>
    void ApplySourceTags(const std::string &source);

    template <class Archive>
    void serialize(Archive &ar, const std::uint32_t version)
    {
        if (version < 6)
        {
            // 旧来の描画システム用の設定は使わない。既定値から始める
            *this = ShaderSettings{};
            return;
        }

        ar(
            CEREAL_NVP(z_test),
            CEREAL_NVP(z_write),
            CEREAL_NVP(cull),
            CEREAL_NVP(blend_mode)
        );
    }
};
}

constexpr const char *ZTestNames[] = {
    "Less",
    "Greater",
    "LEqual",
    "GEqual",
    "Equal",
    "NotEqual",
    "Always",
    "Never",
};
constexpr const char *ZWriteNames[] = {
    "On",
    "Off",
};
constexpr const char *CullNames[] = {
    "Back",
    "Front",
    "Off",
};
constexpr const char *BlendModeNames[] = {
    "Opaque",
    "Alpha",
    "Additive",
};

CEREAL_CLASS_VERSION(engine::ShaderSettings, engine::ShaderSettings::kSerializationVersion)
