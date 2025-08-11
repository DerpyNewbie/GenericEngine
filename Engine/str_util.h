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
/// 指定されたコードページの文字列をワイド文字列(wstring)に変換します。
/// </summary>
/// <param name="string">変換する入力文字列。</param>
/// <param name="src_code_page">入力文字列のソースコードページ。指定されない場合は CP_UTF8 がデフォルトとして使用されます。</param>
/// <returns>入力文字列を変換したワイド文字列(wstring)表現を返します。</returns>
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
/// 指定された wstring を指定のコードページに変換します。
/// </summary>
/// <param name="wstring">変換する入力 wstring。</param>
/// <param name="dst_code_page">返される文字列の出力コードページ。指定されない場合は CP_UTF8 がデフォルトとして使用されます。</param>
/// <returns>入力 wstring を変換した文字列表現を返します。</returns>
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