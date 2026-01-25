#pragma once

namespace engine
{
class EffekseerEffect final : public Object, public Inspectable
{
public:
    std::string path;

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(CEREAL_NVP(path));
    }
};
}

CEREAL_CLASS_VERSION(engine::EffekseerEffect, 1)