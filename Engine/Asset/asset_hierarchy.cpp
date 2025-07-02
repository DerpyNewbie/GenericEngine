#include "pch.h"
#include "asset_hierarchy.h"

namespace engine
{
bool AssetHierarchy::IsFile() const
{
    return m_is_file_;
}
bool AssetHierarchy::IsDirectory() const
{
    return m_is_directory_;
}
}