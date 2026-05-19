#include "pch.h"

#include "render_texture.h"
#include "application.h"
#include "logger.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include <directx/d3dx12_barriers.h>

CEREAL_REGISTER_TYPE(engine::RenderTexture)

void engine::RenderTexture::OnInspectorGui()
{
    //TODO : ここにWidthとHeightと出来ればFormatのPropertyField書いといてね
}