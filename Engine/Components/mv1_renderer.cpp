#include "pch.h"

#include "mv1_renderer.h"

#include "dxlib_helper.h"
#include "game_object.h"
#include "transform.h"

void engine::MV1Renderer::OnDraw()
{
    const MATRIX mat = GameObject()->Transform()->LocalToWorld();
    MV1SetMatrix(model_handle, mat);
    // DxLibHelper::DrawModelFrames(model_handle, Name().c_str());
    MV1DrawModel(model_handle);
}