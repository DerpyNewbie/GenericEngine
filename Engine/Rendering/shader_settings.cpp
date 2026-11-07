#include "pch.h"
#include "shader_settings.h"

namespace engine
{
namespace
{
/// <summary>
/// "// @Key Value" の Value を取り出します。無い場合は空文字を返します。
/// </summary>
std::string FindTag(const std::string &source, const std::string &key)
{
    const auto tag = "@" + key;
    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line))
    {
        const auto comment = line.find("//");
        if (comment == std::string::npos)
            continue;

        std::istringstream words(line.substr(comment + 2));
        std::string word;
        std::string value;
        if (words >> word && word == tag && words >> value)
            return value;
    }
    return {};
}

/// <summary>
/// 名前の一覧から値を探し、見つかった場合はその番号を書き込みます。
/// </summary>
template <size_t N>
void ApplyTag(const std::string &source, const std::string &key, const char *const (&names)[N], int &out)
{
    const auto value = FindTag(source, key);
    if (value.empty())
        return;

    for (int i = 0; i < static_cast<int>(N); ++i)
    {
        if (_stricmp(value.c_str(), names[i]) == 0)
        {
            out = i;
            return;
        }
    }
    Logger::Warn<ShaderSettings>("Unknown value '%s' for @%s", value.c_str(), key.c_str());
}
}

void ShaderSettings::ApplySourceTags(const std::string &source)
{
    ApplyTag(source, "ZTest", ZTestNames, z_test);
    ApplyTag(source, "ZWrite", ZWriteNames, z_write);
    ApplyTag(source, "Cull", CullNames, cull);
    ApplyTag(source, "Blend", BlendModeNames, blend_mode);
}
}
