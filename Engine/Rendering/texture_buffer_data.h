#pragma once
#include "material_data.h"

namespace engine
{
class TextureBufferData : public MaterialDataBase
{
    friend class ShaderImporter;

    AssetPtr<Texture2D> m_texture_;

public:
    TextureBufferData(const ShaderParameter &shader_param);

    void OnInspectorGui() override;

    void SetTexture(const AssetPtr<Texture2D> &texture);
    AssetPtr<Texture2D> Data();

};
}

CEREAL_CLASS_VERSION(engine::TextureBufferData, 1)