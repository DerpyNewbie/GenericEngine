#pragma once
#include <map>
#include <string>
#include <vector>

struct ShaderObject
{
public:
    enum class ValueType
    {
        Float,
        Color,
        Texture
    };

    std::string Name;
    std::wstring VSPath;
    std::wstring PSPath;
    std::wstring GSPath;
    std::map<std::string,ValueType> MaterialValues;

    void AddMaterialValues(std::string name,ValueType valueType);
};
