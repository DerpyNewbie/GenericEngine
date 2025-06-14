#pragma once
#include "Components/transform.h"

namespace engine
{
class ModelImporter
{
public:
    static std::shared_ptr<GameObject> LoadModelFromMV1(const char* file_path);
};
}