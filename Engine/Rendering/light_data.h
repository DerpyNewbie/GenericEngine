#pragma once

struct alignas(16) LightData
{
    int type = 1;
    int cast_shadow = 0;
    float intensity = 1;
    float range = 10;

    Vector4 pos;
    Quaternion direction;
    Color color;

    float inner_cos;
    float outer_cos;
    float padding[2];

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(type),
           CEREAL_NVP(cast_shadow),
           CEREAL_NVP(intensity),
           CEREAL_NVP(range),
           CEREAL_NVP(pos),
           CEREAL_NVP(direction),
           CEREAL_NVP(color),
           CEREAL_NVP(inner_cos),
           CEREAL_NVP(outer_cos));
    }
};