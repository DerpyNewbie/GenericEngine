#pragma once
#include "renderer.h"

namespace engine
{
class MV1Renderer : public Renderer
{
public:
    int model_handle;

    void OnDraw() override;
};
}