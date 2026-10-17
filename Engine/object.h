#pragma once

#include "enable_shared_from_base.h"

namespace engine
{
/// <summary>
/// Engineが管理するすべてのObjectの基底クラスです。Guidと名前を持ち、生成と破棄が管理されます。
/// </summary>
class Object : public enable_shared_from_base<Object>
{
    friend class Engine;
    friend struct AssetDescriptor;
    friend cereal::access;

    inline static bool m_in_gc_time_;
    inline static unsigned int m_last_instantiated_name_count_;
    inline static unsigned int m_last_immediately_destroyed_objects_;
    inline static std::unordered_map<xg::Guid, std::shared_ptr<Object>> m_objects_;
    inline static std::vector<std::weak_ptr<Object>> m_deserialized_objects_;

    xg::Guid m_guid_;
    bool m_is_destroying_ = false;
    std::string m_name_ = "Unknown Object";

    /// <summary>
    /// Destroyが呼ばれたObjectのOnDestroyを呼び出し、管理対象から取り除きます。フレームの最後に呼ばれます。
    /// </summary>
    static void GarbageCollect();
    /// <summary>
    /// Deserializeされた後まだ通知されていないObjectのOnDeserializedを呼び出します。
    /// </summary>
    static void InvokeOnDeserialized();
    /// <summary>
    /// "Instantiated Object N"の形式で連番の名前を生成します。
    /// </summary>
    static std::string GenerateName();
    /// <summary>
    /// 既存のObjectと重複しないGuidを生成します。
    /// </summary>
    static xg::Guid GenerateGuid();

    /// <summary>
    /// Guidを変更し、管理用のMapの登録も新しいGuidに更新します。
    /// </summary>
    /// <param name="new_guid">新しいGuid</param>
    void SetGuid(xg::Guid new_guid);

public:
    Object() = default;
    virtual ~Object() = default;

    /// <summary>
    /// Instantiateで生成された直後に呼ばれます。
    /// </summary>
    virtual void OnConstructed()
    { }

    /// <summary>
    /// Deserializeされた後、そのフレームの終わりに呼ばれます。
    /// </summary>
    virtual void OnDeserialized()
    { }

    /// <summary>
    /// 破棄される時に呼ばれます。
    /// </summary>
    virtual void OnDestroy()
    { }

    /// <summary>
    /// このObjectのGuidを取得します。
    /// </summary>
    [[nodiscard]] xg::Guid Guid() const;

    /// <summary>
    /// このObjectの名前を取得します。
    /// </summary>
    [[nodiscard]] std::string Name() const;
    /// <summary>
    /// このObjectの名前を設定します。
    /// </summary>
    /// <param name="name">新しい名前</param>
    void SetName(const std::string &name);

    /// <summary>
    /// Destroyが呼ばれ、破棄待ち(または破棄済み)であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsDestroying() const;
    /// <summary>
    /// 自身をDestroyします。
    /// </summary>
    void DestroyThis();

    /// <summary>
    /// Objectを破棄待ちにします。実際の破棄はフレームの最後に行われます。
    /// </summary>
    /// <param name="obj">破棄するObject</param>
    static void Destroy(const std::shared_ptr<Object> &obj);
    /// <summary>
    /// Objectを即座に破棄します。GC中やUpdate / FixedUpdate中に呼ばれた場合は、通常のDestroyとして扱われます。
    /// </summary>
    /// <param name="obj">破棄するObject</param>
    static void DestroyImmediate(const std::shared_ptr<Object> &obj);

    /// <summary>
    /// GuidからObjectを取得します。
    /// </summary>
    /// <param name="guid">探すObjectのGuid</param>
    /// <returns>見つからない場合 nullptr</returns>
    [[nodiscard]] static std::shared_ptr<Object> Find(const xg::Guid &guid);

    /// <summary>
    /// 存在するObjectのうち、T にCastできるものをすべて取得します。
    /// </summary>
    template <class T>
    [[nodiscard]] static std::vector<std::shared_ptr<T>> FindByType()
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");
        std::vector<std::shared_ptr<T>> result;
        for (auto &obj : m_objects_ | std::views::values)
        {
            auto casted_obj = std::dynamic_pointer_cast<T>(obj);
            if (casted_obj != nullptr)
            {
                result.push_back(casted_obj);
            }
        }

        return result;
    }

    /// <summary>
    /// 名前とGuidを指定して T のObjectを生成し、OnConstructedを呼び出します。
    /// </summary>
    /// <param name="name">Objectの名前</param>
    /// <param name="guid">ObjectのGuid</param>
    template <class T>
    static std::shared_ptr<T> Instantiate(const std::string &name, const xg::Guid &guid)
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");
        auto obj = std::make_shared<T>();
        auto ptr = std::dynamic_pointer_cast<Object>(obj);
        ptr->SetName(name);
        ptr->m_guid_ = guid;

        m_objects_[guid] = obj;

        ptr->OnConstructed();
        return obj;
    }

    /// <summary>
    /// 名前を指定して T のObjectを生成します。Guidは自動で生成されます。
    /// </summary>
    /// <param name="name">Objectの名前</param>
    template <class T>
    static std::shared_ptr<T> Instantiate(const std::string &name)
    {
        return Instantiate<T>(name, GenerateGuid());
    }

    /// <summary>
    /// Guidを指定して T のObjectを生成します。名前は"Unnamed Object"になります。
    /// </summary>
    /// <param name="guid">ObjectのGuid</param>
    template <class T>
    static std::shared_ptr<T> Instantiate(const xg::Guid &guid)
    {
        return Instantiate<T>("Unnamed Object", guid);
    }

    /// <summary>
    /// T のObjectを生成します。名前とGuidは自動で生成されます。
    /// </summary>
    template <class T>
    static std::shared_ptr<T> Instantiate()
    {
        return Instantiate<T>(GenerateName(), GenerateGuid());
    }

    /// <summary>
    /// Objectを複製します。Serializeした結果のGuidを新しいものに置き換えてDeserializeすることで複製します。
    /// </summary>
    /// <param name="original">複製元のObject</param>
    /// <returns>複製されたObject。失敗した場合 nullptr</returns>
    static std::shared_ptr<Object> Instantiate(const std::shared_ptr<Object> &original);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        bool was_just_deserialized = false;
        if constexpr (Archive::is_loading::value)
        {
            was_just_deserialized = m_guid_ == xg::Guid() && m_name_ == "Unknown Object";
        }

        ar(CEREAL_NVP(m_guid_), CEREAL_NVP(m_name_));

        if constexpr (Archive::is_loading::value)
        {
            m_objects_[m_guid_] = shared_from_this();
            if (was_just_deserialized)
            {
                m_deserialized_objects_.emplace_back(shared_from_this());
            }
        }
    }

    /// <summary>
    /// 自身と同じObjectであるかどうかを判定します。
    /// </summary>
    /// <param name="rhs">比較するObject</param>
    [[nodiscard]] bool Equals(const Object *rhs) const
    {
        return Equals(this, rhs);
    }

    /// <summary>
    /// 2つが同じObjectであるかどうかを判定します。同じポインタであるか、Guidが等しい場合に true を返します。
    /// </summary>
    [[nodiscard]] static bool Equals(const Object *a, const Object *b)
    {
        return a == b || (a != nullptr && b != nullptr && a->Guid() == b->Guid());
    }

    /// <summary>
    /// 存在するすべてのObjectのMapのコピーを取得します。
    /// </summary>
    [[nodiscard]] static std::unordered_map<xg::Guid, std::shared_ptr<Object>> AllObjects()
    {
        return m_objects_;
    }
};
}

CEREAL_CLASS_VERSION(engine::Object, 1)