#pragma once
#include "inspectable_asset.h"

namespace engine
{
class DummyAsset : public InspectableAsset
{
    void OnInspectorGui() override;
};
}