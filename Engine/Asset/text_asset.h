#pragma once
#include "inspectable_asset.h"

namespace engine
{
class TextAsset final : public InspectableAsset
{
public:
    std::string content;
    std::unordered_map<std::string, std::string> key_value_pairs;

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(content);
    }
};
}