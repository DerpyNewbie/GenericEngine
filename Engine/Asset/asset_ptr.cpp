#include "pch.h"
#include "asset_ptr.h"

#include "asset_database.h"
#include "asset_descriptor.h"
#include "Importer/asset_importer.h"

namespace engine
{
const xg::Guid IAssetPtr::kNullGuid = xg::Guid();
const xg::Guid IAssetPtr::kSceneRefGuid = xg::Guid("ffffffff-ffff-ffff-ffff-ffffffffffff");

IAssetPtr::IAssetPtr()
{
    m_guid_ = kNullGuid;
}
IAssetPtr IAssetPtr::FromScene(std::shared_ptr<Object> ptr)
{
    return {ptr, kSceneRefGuid};
}
IAssetPtr IAssetPtr::FromAssetDescriptor(const std::shared_ptr<AssetDescriptor> &asset)
{
    return {asset->managed_object, asset->guid};
}
xg::Guid IAssetPtr::Guid() const
{
    return m_guid_;
}
std::shared_ptr<Object> IAssetPtr::Lock()
{
    if (m_ptr_.expired())
    {
        auto asset_descriptor = AssetDatabase::GetAssetDescriptor(m_guid_);
        if (asset_descriptor == nullptr)
        {
            throw std::runtime_error("Asset not found");
        }

        auto importer = AssetImporter::GetAssetImporter(asset_descriptor->type_hint);
        if (importer == nullptr)
        {
            throw std::runtime_error("Asset importer not found");
        }

        m_ptr_ = importer->Import(asset_descriptor.get());
    }
    return m_ptr_.lock();
}
bool IAssetPtr::IsFileReference() const
{
    return m_guid_ != kNullGuid && m_guid_ != kSceneRefGuid;
}
bool IAssetPtr::IsSceneReference() const
{
    return m_guid_ == kSceneRefGuid;
}
bool IAssetPtr::IsLoaded() const
{
    return !m_ptr_.expired();
}
bool IAssetPtr::IsNull() const
{
    return m_guid_ == kNullGuid;
}
}

CEREAL_REGISTER_TYPE(engine::IAssetPtr)