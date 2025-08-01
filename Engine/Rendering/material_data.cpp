#include "pch.h"
#include "material_data.h"


CEREAL_REGISTER_TYPE(engine::IMaterialData)

CEREAL_REGISTER_TYPE(engine::MaterialData<bool>)

CEREAL_REGISTER_TYPE(engine::MaterialData<int>)

CEREAL_REGISTER_TYPE(engine::MaterialData<float>)

CEREAL_REGISTER_TYPE(engine::MaterialData<Matrix>)

CEREAL_REGISTER_TYPE(engine::MaterialData<engine::AssetPtr<Texture2D>>)