#pragma once

namespace engine
{
class DummyAsset : public Object, public Inspectable
{
    void OnInspectorGui() override;
};
}