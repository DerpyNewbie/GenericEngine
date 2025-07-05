#include "pch.h"
#include "MaterialData.h"
#include <cereal/types/polymorphic.hpp>

CEREAL_REGISTER_TYPE(engine::MaterialData<int>)
CEREAL_REGISTER_TYPE(engine::MaterialData<float>)
CEREAL_REGISTER_TYPE(engine::MaterialData<std::weak_ptr<Texture2D>>)