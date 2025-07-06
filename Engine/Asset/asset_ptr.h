#pragma once
#include "crossguid/guid.hpp"

namespace engine
{
struct AssetDescriptor;
class Object;

struct IAssetPtr
{
protected:
    std::weak_ptr<Object> m_ptr_;
    xg::Guid m_guid_;

    IAssetPtr(const std::shared_ptr<Object> &ptr, const xg::Guid guid) : m_ptr_(ptr), m_guid_(guid)
    {}

public:
    static const xg::Guid kNullGuid;
    static const xg::Guid kSceneRefGuid;

    virtual ~IAssetPtr() = default;

    IAssetPtr();

    static IAssetPtr FromScene(std::shared_ptr<Object> ptr);

    static IAssetPtr FromAssetDescriptor(const std::shared_ptr<AssetDescriptor> &asset);

    [[nodiscard]] xg::Guid Guid() const;

    [[nodiscard]] virtual std::shared_ptr<Object> Lock();

    [[nodiscard]] bool IsFileReference() const;

    [[nodiscard]] bool IsSceneReference() const;

    [[nodiscard]] bool IsLoaded() const;

    [[nodiscard]] bool IsNull() const;

    bool operator==(const IAssetPtr &other) const
    {
        return m_guid_ == other.m_guid_;
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m_guid_);
        auto _ = Lock(); // try to import the object associated with guid
    }
};

template <typename T> requires std::is_base_of_v<Object, T>
struct AssetPtr : IAssetPtr
{
    std::shared_ptr<T> CastedLock()
    {
        if (m_ptr_.expired())
            return nullptr;

        return std::dynamic_pointer_cast<T>(m_ptr_.lock());
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<IAssetPtr>(this));
        auto _ = Lock(); // try to import the object associated with guid
    }
};
}