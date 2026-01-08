#pragma once
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/font_data.h"

namespace engine
{

class TextRenderer : public Component
{
    friend class RenderPipeline;
    //HACK: あんまりよくないかも
    inline static std::unordered_set<std::shared_ptr<TextRenderer>> m_text_renderers_;

public:
    Vector2 position;
    std::string string;
    Color color;
    float rotation;
    float scale;
    Vector2 origin;
    AssetPtr<FontData> font_data;

    void OnInspectorGui() override;

    void OnEnabled() override;
    void OnDisabled() override;

    void Render();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
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