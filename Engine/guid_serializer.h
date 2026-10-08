#pragma once

namespace cereal
{
/// <summary>
/// cereal用の関数です。Guidを文字列としてSerializeします。
/// </summary>
template <class Archive>
std::string save_minimal(const Archive &, const xg::Guid &guid)
{
    return guid.str();
}
/// <summary>
/// cereal用の関数です。文字列からGuidをDeserializeします。
/// </summary>
template <class Archive>
void load_minimal(const Archive &, xg::Guid &guid, const std::string &value)
{
    auto next = xg::Guid(value);
    guid.swap(next);
}
}