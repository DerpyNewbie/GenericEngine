#pragma once
#include "inspectable_asset.h"

namespace engine
{
class TextAsset final : public InspectableAsset
{
public:
    std::string content;

    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(content);
    }
};
}