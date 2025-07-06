#pragma once
#include "asset_ptr.h"

namespace engine
{
struct AssetDescriptor : Object
{
private:
    friend class AssetDatabase;

    static std::filesystem::path ToMetaFilePath(const std::filesystem::path &path);

    void Write(const std::filesystem::path &path) const;
    static std::shared_ptr<AssetDescriptor> Read(const std::filesystem::path &path);

public:
    xg::Guid guid = xg::Guid();
    std::filesystem::path path = "";
    std::string type_hint;
    std::shared_ptr<Object> managed_object = nullptr; // can be null

    IAssetPtr ToAssetPtr();
};
}