#include "engine_util.h"

#include "logger.h"
#include "Components/component.h"

#include <cassert>
#include <codecvt>
#include <locale>
#include <vector>

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
void EngineUtil::ToFloat3(float buff[3], const VECTOR vec)
{
    buff[0] = vec.x;
    buff[1] = vec.y;
    buff[2] = vec.z;
}
std::wstring EngineUtil::ShiftJisToUtf16(const std::string &shift_jis_string)
{
    const int size = MultiByteToWideChar(CP_ACP, 0, shift_jis_string.data(), -1, nullptr, 0);
    std::vector buffer(size, L'\0');
    if (MultiByteToWideChar(CP_ACP, 0, shift_jis_string.data(), -1, buffer.data(), static_cast<int>(buffer.size())) == 0)
    {
        assert(FALSE && "Failed to convert ShiftJis to UTF16");
    }
    buffer.resize(std::char_traits<wchar_t>::length(buffer.data()));
    buffer.shrink_to_fit();
    return {buffer.begin(), buffer.end()};
}
std::string EngineUtil::Utf16ToUtf8(const std::wstring &utf16_string)
{
    const int size = WideCharToMultiByte(CP_UTF8, 0, utf16_string.data(), -1, nullptr, 0, nullptr, nullptr);
    std::vector buffer(size, '\0');
    if (WideCharToMultiByte(CP_UTF8, 0, utf16_string.data(), -1, buffer.data(), static_cast<int>(buffer.size()), nullptr, nullptr) == 0)
    {
        assert(FALSE && "Failed to convert ShiftJis to UTF8");
    }
    buffer.resize(std::char_traits<char>::length(buffer.data()));
    buffer.shrink_to_fit();
    return {buffer.begin(), buffer.end()};
}
std::string EngineUtil::ShiftJisToUtf8(const std::string &shift_jis_string)
{
    return Utf16ToUtf8(ShiftJisToUtf16(shift_jis_string));
}
}