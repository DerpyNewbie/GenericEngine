#pragma once
#include "Components/transform.h"

namespace engine
{
class ModelImporter
{
public:
    static std::shared_ptr<GameObject> LoadModelFromFBX(const char *file_path);
};
}