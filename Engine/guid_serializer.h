#pragma once
#include <crossguid/guid.hpp>

namespace cereal
{
template <class Archive>
std::string save_minimal(const Archive &, const xg::Guid &guid)
{
    return guid.str();
}
template <class Archive>
void load_minimal(const Archive &, xg::Guid &guid, const std::string &value)
{
    auto next = xg::Guid(value);
    guid.swap(next);
}
}