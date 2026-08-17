#pragma once
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/font_data.h"

namespace engine
{

class TextRenderer : public Renderer
{
public:
    Vector2 position;
    AssetPtr<FontData> font_data;
    std::string string;
    Color color;
    float rotation;
    Vector2 origin;
    float scale = 1.0f;
    uint16_t render_queue;

    void OnInspectorGui() override;
    void Render() override;

    Matrix BoundsOrigin() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Renderer>(this),
            CEREAL_NVP(position),
            CEREAL_NVP(font_data),
            CEREAL_NVP(color),
            CEREAL_NVP(string)
        );

        if (version >= 2)
        {
            ar(
                CEREAL_NVP(rotation),
                CEREAL_NVP(origin),
                CEREAL_NVP(scale)
            );
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::TextRenderer, 2)