#pragma once

namespace engine
{
class TextAsset final : public Object, public Inspectable
{
public:
    std::string content;
    std::unordered_map<std::string, std::string> key_value_pairs;

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(CEREAL_NVP(content));
    }
};
}

CEREAL_CLASS_VERSION(engine::TextAsset, 1)