#include "pch.h"
#include "asset_ptr.h"

#include "asset_database.h"
#include "asset_descriptor.h"
#include "Components/component.h"
#include "game_object.h"

namespace engine
{
const xg::Guid IAssetPtr::kNullGuid = xg::Guid();

IAssetPtr::IAssetPtr()
{
    m_guid_ = kNullGuid;
}

IAssetPtr IAssetPtr::FromManaged(const std::weak_ptr<Object> &ptr)
{
    auto lock = ptr.lock();
    return {
        ptr,
        {},
        lock != nullptr ? lock->Guid() : kNullGuid,
        lock != nullptr ? AssetPtrType::kExternalReference : AssetPtrType::kNull
    };
}

IAssetPtr IAssetPtr::FromInstance(const std::shared_ptr<Object> &ptr)
{
    return {
        {},
        ptr,
        ptr != nullptr ? ptr->Guid() : kNullGuid,
        ptr != nullptr ? AssetPtrType::kStoredReference : AssetPtrType::kNull
    };
}

IAssetPtr IAssetPtr::FromAssetDescriptor(const std::shared_ptr<AssetDescriptor> &asset)
{
    return {
        asset->managed_object,
        {},
        asset->guid,
        AssetPtrType::kExternalReference
    };
}

xg::Guid IAssetPtr::Guid() const
{
    return m_guid_;
}

std::shared_ptr<Object> IAssetPtr::Lock()
{
    switch (m_type_)
    {
    default:
    case AssetPtrType::kNull:
        return nullptr;
    case AssetPtrType::kStoredReference:
        return m_stored_reference_;
    case AssetPtrType::kExternalReference: {
        const auto lock = m_external_reference_.lock();
        if (lock == nullptr)
        {
            auto obj = Object::Find(m_guid_);
            if (obj != nullptr)
            {
                m_external_reference_ = obj;
                return obj;
            }

            const auto asset_descriptor = AssetDatabase::GetAssetDescriptor(m_guid_);
            if (asset_descriptor != nullptr)
            {
                asset_descriptor->Reload();
                m_external_reference_ = asset_descriptor->managed_object;
                return asset_descriptor->managed_object;
            }

            // missing reference
            return nullptr;
        }

        return lock;
    }
    }
}

std::string IAssetPtr::Name() const
{
    const auto locked = m_external_reference_.lock();
    if (locked != nullptr)
    {
        const auto component = std::dynamic_pointer_cast<Component>(locked);
        if (component != nullptr && component->GameObject() != nullptr)
        {
            return component->GameObject()->Name() + " (" + component->Name() + ")";
        }
        return locked->Name();
    }

    if (IsNull())
    {
        return "(None)";
    }

    return "(Missing)";
}
bool IAssetPtr::IsNull() const
{
    return m_guid_ == kNullGuid;
}
bool IAssetPtr::IsLoaded()
{
    return Lock() != nullptr;
}
}

CEREAL_REGISTER_TYPE(engine::IAssetPtr)