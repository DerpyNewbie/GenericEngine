#include "ShaderObject.h"

void ShaderObject::AddMaterialValues(std::string name, ValueType valueType)
{
    MaterialValues.emplace(name, valueType);
}
