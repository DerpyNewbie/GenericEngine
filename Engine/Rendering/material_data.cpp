#include "pch.h"
#include "material_data.h"


CEREAL_REGISTER_TYPE(engine::IMaterialData)

CEREAL_REGISTER_TYPE(engine::MaterialData<bool>)

CEREAL_REGISTER_TYPE(engine::MaterialData<int>)

CEREAL_REGISTER_TYPE(engine::MaterialData<float>)

CEREAL_REGISTER_TYPE(engine::MaterialData<Color>)

CEREAL_REGISTER_TYPE(engine::MaterialData<Vector2>)

CEREAL_REGISTER_TYPE(engine::MaterialData<Vector3>)

CEREAL_REGISTER_TYPE(engine::MaterialData<engine::AssetPtr<engine::Texture2D>>)