#pragma once
#include "crossguid/guid.hpp"

namespace engine
{
struct AssetDescriptor;
class Object;

/// <summary>
/// AssetPtrが持つ参照の種類です。
/// </summary>
enum class AssetPtrType
{
    kNull,
    kStoredReference,
    kExternalReference
};

/// <summary>
/// Objectへの参照をGuidとともに保持する、型を持たないAssetPtrの基底構造体です。
/// </summary>
struct IAssetPtr
{
protected:
    mutable std::weak_ptr<Object> m_external_reference_;
    std::shared_ptr<Object> m_stored_reference_;
    xg::Guid m_guid_;
    AssetPtrType m_type_ = AssetPtrType::kNull;

    /// <summary>
    /// 各メンバーを直接指定してAssetPtrを作成します。
    /// </summary>
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

    /// <summary>
    /// 何も参照していないAssetPtrを作成します。
    /// </summary>
    IAssetPtr();

    /// <summary>
    /// 他の場所で管理されているObjectを参照するAssetPtr(kExternalReference)を作成します。AssetPtr自身はObjectを保持しません。
    /// </summary>
    /// <param name="ptr">参照するObject</param>
    static IAssetPtr FromManaged(const std::weak_ptr<Object> &ptr);

    /// <summary>
    /// Objectを自身で保持するAssetPtr(kStoredReference)を作成します。
    /// </summary>
    /// <param name="ptr">保持するObject</param>
    static IAssetPtr FromInstance(const std::shared_ptr<Object> &ptr);

    /// <summary>
    /// AssetのMainObjectを参照するAssetPtrを作成します。
    /// </summary>
    /// <param name="asset">参照するAssetのDescriptor</param>
    static IAssetPtr FromAssetDescriptor(const std::shared_ptr<AssetDescriptor> &asset);

    /// <summary>
    /// 参照先のGuidを取得します。
    /// </summary>
    [[nodiscard]] xg::Guid Guid() const;

    /// <summary>
    /// 表示用の名前を取得します。何も参照していない場合は"(None)"、参照先が見つからない場合は"(Missing)"になります。
    /// </summary>
    [[nodiscard]] std::string Name() const;

    /// <summary>
    /// 何も参照していない、または参照先が破棄待ちであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsNone() const;

    /// <summary>
    /// 参照は設定されているが、参照先が見つからない状態であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsMissing() const;

    /// <summary>
    /// IsNoneまたはIsMissingであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsNull() const;

    /// <summary>
    /// 参照先のObjectを取得します。kExternalReferenceで参照が切れている場合はGuidから探し直し、AssetであればImportします。
    /// </summary>
    /// <returns>見つからない場合 nullptr</returns>
    [[nodiscard]] virtual std::shared_ptr<Object> Lock() const;

    /// <summary>
    /// 参照先のGuidが等しいかどうかを比較します。
    /// </summary>
    bool operator==(const IAssetPtr &other) const
    {
        return m_guid_ == other.m_guid_;
    }

    /// <summary>
    /// IsNullであるかどうかを判定します。
    /// </summary>
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

/// <summary>
/// T型のObjectへの参照を保持するAssetPtrです。
/// </summary>
template <typename T> requires std::is_base_of_v<Object, T>
struct AssetPtr : IAssetPtr
{
private:
    mutable std::weak_ptr<T> m_cached_ptr_reference_;

    /// <summary>
    /// 各メンバーを直接指定してAssetPtrを作成します。
    /// </summary>
    AssetPtr(
        const std::weak_ptr<Object> &weak_ptr,
        const std::shared_ptr<Object> &shared_ptr,
        const xg::Guid guid,
        const AssetPtrType type
        ) :
        IAssetPtr(weak_ptr, shared_ptr, guid, type)
    { }

    /// <summary>
    /// IAssetPtrからAssetPtrを作成します。
    /// </summary>
    AssetPtr(const IAssetPtr &ptr) :
        IAssetPtr(ptr)
    { }

public:
    AssetPtr() = default;

    /// <summary>
    /// 参照先のObjectを T にCastして取得します。
    /// </summary>
    /// <returns>見つからない、またはCastできない場合 nullptr</returns>
    std::shared_ptr<T> CastedLock() const
    {
        auto locked = Lock();
        if (locked != m_cached_ptr_reference_.lock())
        {
            m_cached_ptr_reference_ = std::dynamic_pointer_cast<T>(locked);
        }

        return m_cached_ptr_reference_.lock();
    }

    /// <summary>
    /// IAssetPtrを T のAssetPtrに変換します。
    /// </summary>
    /// <param name="ptr">変換元のIAssetPtr</param>
    static AssetPtr FromIAssetPtr(IAssetPtr ptr)
    {
        return {ptr};
    }

    /// <summary>
    /// 他の場所で管理されているObjectを参照するAssetPtr(kExternalReference)を作成します。AssetPtr自身はObjectを保持しません。
    /// </summary>
    /// <param name="ptr">参照するObject</param>
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

    /// <summary>
    /// Objectを自身で保持するAssetPtr(kStoredReference)を作成します。
    /// </summary>
    /// <param name="ptr">保持するObject</param>
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

    /// <summary>
    /// 参照先のObjectを T にCastしてアクセスします。
    /// </summary>
    std::shared_ptr<T> operator->() const
    {
        return CastedLock();
    }

    /// <summary>
    /// 参照先のGuidが等しいかどうかを比較します。
    /// </summary>
    bool operator==(const AssetPtr other) const
    {
        return Guid() == other.Guid();
    }

    /// <summary>
    /// 参照先が指定されたObjectと同じGuidであるかどうかを比較します。nullptrとの比較はIsNullと同じです。
    /// </summary>
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