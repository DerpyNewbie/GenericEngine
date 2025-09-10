#include "pch.h"
#include "dummy_asset.h"

namespace engine
{
void DummyAsset::OnInspectorGui()
{
    ImGui::Text("I'm a dummy!");
}
}