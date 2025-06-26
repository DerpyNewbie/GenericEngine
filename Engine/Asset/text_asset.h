#pragma once
#include "inspectable_asset.h"
#include "object.h"

namespace engine
{
class TextAsset final : public InspectableAsset
{
public:
    std::string content;

    void OnInspectorGui() override;
};
}