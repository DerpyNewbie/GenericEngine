#pragma once
#include "color.h"
#include "renderer.h"
#include <DxLib.h>

namespace engine
{
class CubeRenderer final : public Renderer
{
public:
    VECTOR extent = VGet(1, 1, 1);
    Color diffuse_color = Color{0xffffffff};
    Color specular_color = Color{0xffffffff};
    bool fill = true;

    void OnDraw() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        Object::serialize(ar);
    }
};
}