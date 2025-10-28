#pragma once
#include <string>
#include <winnls.h>
#include <cassert>
#include <vector>

#define CP_SHIFT_JIS 932  // NOLINT(modernize-macro-to-enum)

// ReSharper disable once CppInconsistentNaming
namespace StringUtil
{
/// <summary>
/// Converts a string with the specified code page to a wide string (wstring).
/// </summary>
/// <param name="string">The input string to convert.</param>
/// <param name="src_code_page">The source code page of the input string. If not specified, CP_UTF8 is used as default.</param>
/// <returns>Returns the wide string (wstring) representation of the converted input string.</returns>
static std::wstring ConvertToWString(const std::string &string, const unsigned int src_code_page = CP_UTF8)
{
    const int size = MultiByteToWideChar(src_code_page, 0, string.data(), -1, nullptr, 0);
    std::vector buffer(size, L'\0');
    if (MultiByteToWideChar(src_code_page, 0, string.data(), -1, buffer.data(),
                            static_cast<int>(buffer.size())) == 0)
    {
        assert(FALSE && "Failed to convert string to wstring");
    }
    buffer.resize(std::char_traits<wchar_t>::length(buffer.data()));
    buffer.shrink_to_fit();
    return {buffer.begin(), buffer.end()};
}

/// <summary>
/// Converts the specified wstring to the specified code page.
/// </summary>
/// <param name="wstring">The input wstring to convert.</param>
/// <param name="dst_code_page">The output code page for the returned string. If not specified, CP_UTF8 is used as default.</param>
/// <returns>Returns the string representation of the converted input wstring.</returns>
static std::string ConvertToString(const std::wstring &wstring, const unsigned int dst_code_page = CP_UTF8)
{
    const int size = WideCharToMultiByte(dst_code_page, 0, wstring.data(), -1, nullptr, 0, nullptr, nullptr);
    std::vector buffer(size, '\0');
    if (WideCharToMultiByte(dst_code_page, 0, wstring.data(), -1, buffer.data(), static_cast<int>(buffer.size()),
                            nullptr, nullptr) == 0)
    {
        assert(FALSE && "Failed to convert wstring to string");
    }
    buffer.resize(std::char_traits<char>::length(buffer.data()));
    buffer.shrink_to_fit();
    return {buffer.begin(), buffer.end()};
}
static std::wstring ShiftJisToUtf16(const std::string &shift_jis_string)
{
    return ConvertToWString(shift_jis_string, CP_SHIFT_JIS);
}
static std::wstring Utf8ToUtf16(const std::string &utf8_string)
{
    return ConvertToWString(utf8_string, CP_UTF8);
}
static std::string Utf16ToUtf8(const std::wstring &utf16_string)
{
    return ConvertToString(utf16_string, CP_UTF8);
}
static std::string Utf16ToShiftJis(const std::wstring &utf16_string)
{
    return ConvertToString(utf16_string, CP_SHIFT_JIS);
}
static std::string ShiftJisToUtf8(const std::string &shift_jis_string)
{
    return Utf16ToUtf8(ShiftJisToUtf16(shift_jis_string));
}
static std::string Utf8ToShiftJis(const std::string &utf8_string)
{
    return Utf16ToShiftJis(Utf8ToUtf16(utf8_string));
}
static std::string DurationToString(std::chrono::milliseconds ms)
{
    const auto days = duration_cast<std::chrono::days>(ms);
    ms -= days;

    const auto hours = duration_cast<std::chrono::hours>(ms);
    ms -= hours;

    const auto minutes = duration_cast<std::chrono::minutes>(ms);
    ms -= minutes;

    const auto seconds = duration_cast<std::chrono::seconds>(ms);
    ms -= seconds;

    const auto millis = duration_cast<std::chrono::milliseconds>(ms);

    std::ostringstream oss;
    oss << std::setfill('0');

    bool first = true;
    if (days.count() > 0)
    {
        oss << std::setw(2) << days.count() << ":";
        first = false;
    }

    if (!first || hours.count() > 0)
    {
        oss << std::setw(2) << hours.count() << ":";
        first = false;
    }

    oss << std::setw(2) << minutes.count() << ":"
        << std::setw(2) << seconds.count() << "."
        << std::setw(3) << millis.count();

    return oss.str();
}
}