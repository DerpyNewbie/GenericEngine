#pragma once
#include "asset_descriptor.h"
#include "object.h"

namespace engine
{
class AssetHierarchy : public Object
{
    friend class AssetDatabase;

    bool m_is_file_;
    bool m_is_directory_;

public:
    std::shared_ptr<AssetDescriptor> asset; // can be null
    std::shared_ptr<AssetHierarchy> parent; // can be null at root
    std::vector<std::shared_ptr<AssetHierarchy>> children; // can be empty

    [[nodiscard]] bool IsFile() const;

    [[nodiscard]] bool IsDirectory() const;
};
}