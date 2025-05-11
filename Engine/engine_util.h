#pragma once
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
};
}