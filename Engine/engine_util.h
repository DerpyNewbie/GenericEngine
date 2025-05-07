#pragma once
#include <DxLib.h>

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
    static float *ToFloat3(VECTOR vec);
    static std::wstring ShiftJisToUtf16(const std::string &shift_jis_string);
    static std::string Utf16ToUtf8(const std::wstring &utf16_string);
    static std::string ShiftJisToUtf8(const std::string &shift_jis_string);
};
}