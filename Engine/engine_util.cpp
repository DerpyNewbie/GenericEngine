#include "pch.h"

#include "engine_util.h"

#include "Components/component.h"

namespace engine
{
std::string EngineUtil::GetTypeName(const char *typeid_name)
{
    return Logger::GetTypeName(typeid_name);
}

std::string EngineUtil::GetTypeName(Component *component)
{
    return GetTypeName(typeid(*component).name());
}
std::string EngineUtil::GetTypeName(const std::shared_ptr<Component> &component)
{
    return GetTypeName(typeid(component.get()).name());
}
void EngineUtil::ToFloat3(float buff[3], const Vector3 vec)
{
    buff[0] = vec.x;
    buff[1] = vec.y;
    buff[2] = vec.z;
}
void EngineUtil::ToFloat4(float buff[4], const Color vec)
{
    buff[0] = vec.x;
    buff[1] = vec.y;
    buff[2] = vec.z;
    buff[3] = vec.w;
}
}