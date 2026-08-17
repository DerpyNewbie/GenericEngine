#include "pch.h"
#include "effekseer_effect.h"

#include "gui.h"

void engine::EffekseerEffect::OnInspectorGui()
{
    Gui::ReadOnlyStringField("Path : ", path);
}