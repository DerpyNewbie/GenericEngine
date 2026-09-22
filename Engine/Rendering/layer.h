#pragma once

namespace engine
{
class Layer : public Inspectable
{
    static constexpr uint32_t kLayerCount = sizeof(uint16_t) * 8;

    inline static std::array<std::string, kLayerCount> m_layer_names_;
    uint16_t m_masks_ = 1;

public:
    void OnInspectorGui() override;

    void SetLayer(uint32_t layer, bool enabled);
    bool IsLayerEnabled(uint32_t layer) const;
    bool IsAnyLayerEnabled(const Layer& other) const;
    bool IsAllLayerEnabled(const Layer& other) const;
    
    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(m_layer_names_),
            CEREAL_NVP(m_masks_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Layer, 1)