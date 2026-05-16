#pragma once
#include "crossguid/guid.hpp"

namespace engine
{
struct AssetDescriptor;
class Object;

enum class AssetPtrType
{
    kNull,
    kStoredReference,
    kExternalReference
};

struct IAssetPtr
{
protected:
    mutable std::weak_ptr<Object> m_external_reference_;
    std::shared_ptr<Object> m_stored_reference_;
    xg::Guid m_guid_;
    AssetPtrType m_type_ = AssetPtrType::kNull;

    IAssetPtr(
        const std::weak_ptr<Object> &weak_ptr,
        const std::shared_ptr<Object> &shared_ptr,
        const xg::Guid guid,
        const AssetPtrType type
        ) :
        m_external_reference_(weak_ptr),
        m_stored_reference_(shared_ptr),
        m_guid_(guid),
        m_type_(type)
    { }

public:
    static const xg::Guid kNullGuid;

    virtual ~IAssetPtr() = default;

    IAssetPtr();

    static IAssetPtr FromManaged(const std::weak_ptr<Object> &ptr);

    static IAssetPtr FromInstance(const std::shared_ptr<Object> &ptr);

    static IAssetPtr FromAssetDescriptor(const std::shared_ptr<AssetDescriptor> &asset);

    [[nodiscard]] xg::Guid Guid() const;

    [[nodiscard]] std::string Name() const;

    [[nodiscard]] bool IsNone() const;

    [[nodiscard]] bool IsMissing() const;

    [[nodiscard]] bool IsNull() const;

    [[nodiscard]] virtual std::shared_ptr<Object> Lock() const;

    bool operator==(const IAssetPtr &other) const
    {
        return m_guid_ == other.m_guid_;
    }

    bool operator==(nullptr_t) const
    {
        return IsNull();
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(CEREAL_NVP(m_guid_), CEREAL_NVP(m_type_), CEREAL_NVP(m_stored_reference_));
        auto _ = Lock(); // try to import the object associated with guid
    }
};

template <typename T> requires std::is_base_of_v<Object, T>
struct AssetPtr : IAssetPtr
{
private:
    mutable std::weak_ptr<T> m_cached_ptr_reference_;

    AssetPtr(
        const std::weak_ptr<Object> &weak_ptr,
        const std::shared_ptr<Object> &shared_ptr,
        const xg::Guid guid,
        const AssetPtrType type
        ) :
        IAssetPtr(weak_ptr, shared_ptr, guid, type)
    { }

    AssetPtr(const IAssetPtr &ptr) :
        IAssetPtr(ptr)
    { }

public:
    AssetPtr() = default;

    std::shared_ptr<T> CastedLock() const
    {
        auto locked = Lock();
        if (locked != m_cached_ptr_reference_.lock())
        {
            m_cached_ptr_reference_ = std::dynamic_pointer_cast<T>(locked);
        }

        return m_cached_ptr_reference_.lock();
    }

    static AssetPtr FromIAssetPtr(IAssetPtr ptr)
    {
        return {ptr};
    }

    static AssetPtr FromManaged(const std::weak_ptr<T> &ptr)
    {
        auto lock = ptr.lock();
        return {
            ptr,
            {},
            lock != nullptr ? lock->Guid() : kNullGuid,
            lock != nullptr ? AssetPtrType::kExternalReference : AssetPtrType::kNull
        };
    }

    static AssetPtr FromInstance(const std::shared_ptr<T> &ptr)
    {
        return {
            {},
            ptr,
            ptr != nullptr ? ptr->Guid() : kNullGuid,
            ptr != nullptr ? AssetPtrType::kStoredReference : AssetPtrType::kNull
        };
    }

    // Does the casting implicitly
    // ReSharper disable once CppNonExplicitConversionOperator
    operator std::shared_ptr<T>() const
    {
        return CastedLock();
    }

    // Does the casting to base AssetPtr implicitly. requires T to be inheriting Base, and T and Base must not be the same type. 
    // ReSharper disable once CppNonExplicitConversionOperator
    template <typename Base> requires !std::is_same_v<Base, T> && std::is_base_of_v<Base, T>
    operator AssetPtr<Base>() const
    {
        return AssetPtr<Base>::FromIAssetPtr(*this);
    }

    std::shared_ptr<T> operator->() const
    {
        return CastedLock();
    }

    bool operator==(const AssetPtr other) const
    {
        return Guid() == other.Guid();
    }

    bool operator==(const std::shared_ptr<T> &other) const
    {
        if (other == nullptr)
            return IsNull();

        return Guid() == other->Guid();
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<IAssetPtr>(this));
        auto _ = Lock(); // try to import the object associated with guid
    }
};
}

CEREAL_CLASS_VERSION(engine::IAssetPtr, 1)