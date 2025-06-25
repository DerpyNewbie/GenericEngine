#pragma once
#include "object.h"

namespace engine
{
struct AssetDescriptor
{
    std::filesystem::path path;
    std::shared_ptr<Object> object; // can be null
};
}