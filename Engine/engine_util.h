#pragma once
#include <directxtk12/SimpleMath.h>

#include <string>
#include <memory>

namespace engine
{
class Component;
class EngineUtil
{
public:
    static std::string GetTypeName(const char *typeid_name);
    static std::string GetTypeName(Component *component);
    static std::string GetTypeName(const std::shared_ptr<Component> &component);
    static void ToFloat3(float buff[3], DirectX::SimpleMath::Vector3 vec);
};
}