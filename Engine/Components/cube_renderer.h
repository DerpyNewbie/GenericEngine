#pragma once
#include "renderer.h"
#include "Math/color.h"

#include <DxLib.h>

namespace engine
{
class CubeRenderer final : public Renderer
{
public:
    Vector3 extent = VGet(1, 1, 1);
    Color diffuse_color = Color{0xffffffff};
    Color specular_color = Color{0xffffffff};
    bool fill = true;

    void OnDraw() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(extent),
           CEREAL_NVP(diffuse_color),
           CEREAL_NVP(specular_color),
           CEREAL_NVP(fill));
    }
};
}