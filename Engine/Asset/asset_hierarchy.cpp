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

void AssetHierarchy::OnDestroy()
{
    const auto locked_parent = parent.lock();
    if (locked_parent != nullptr)
    {
        std::erase_if(locked_parent->children, [this](const std::shared_ptr<AssetHierarchy> &child) {
            return child->Guid() == Guid();
        });
    }

    if (asset != nullptr && asset->managed_object != nullptr)
    {
        DestroyImmediate(asset->managed_object);
        asset->managed_object = nullptr;
    }

    for (auto child : children)
    {
        DestroyImmediate(child);
    }
}
}