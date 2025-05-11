#include "engine_util.h"

#include "logger.h"
#include "component.h"

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
}