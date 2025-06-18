#include "pch.h"

#include "mv1_renderer.h"

#include "DxLib/dxlib_helper.h"
#include "DxLib/dxlib_converter.h"
#include "game_object.h"
#include "transform.h"

void engine::MV1Renderer::OnDraw()
{
    const MATRIX mat = DxLibConverter::From(GameObject()->Transform()->LocalToWorld());
    MV1SetMatrix(model_handle, mat);
    // DxLibHelper::DrawModelFrames(model_handle, Name().c_str());
    MV1DrawModel(model_handle);
}