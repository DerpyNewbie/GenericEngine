#pragma once
#include "game_object.h"

namespace engine
{
/// <summary>
/// Componentを名前から生成するFactoryの基底クラスです。登録されたFactoryの一覧も管理します。
/// </summary>
class IComponentFactory
{
    friend class Engine;

    static std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> m_factories_;
    std::string m_name_;
    std::string m_friendly_name_;
    std::string m_category_;

    /// <summary>
    /// Engine標準のComponentのFactoryをすべて登録します。
    /// </summary>
    static void Init();

public:
    virtual ~IComponentFactory() = default;

    /// <summary>
    /// Factoryを作成します。
    /// </summary>
    /// <param name="name">Componentのクラス名(typeidの名前)</param>
    /// <param name="friendly_name">表示用の名前</param>
    /// <param name="category">Componentのカテゴリ</param>
    explicit IComponentFactory(const std::string &name, const std::string &friendly_name, const std::string &category);

    /// <summary>
    /// Componentのクラス名(typeidの名前)を取得します。
    /// </summary>
    std::string Name();
    /// <summary>
    /// 表示用の名前を取得します。
    /// </summary>
    std::string FriendlyName();
    /// <summary>
    /// Componentのカテゴリを取得します。
    /// </summary>
    std::string Category();

    /// <summary>
    /// 指定されたGameObjectに、このFactoryが対象とするComponentを追加します。
    /// </summary>
    virtual void AddComponentTo(std::shared_ptr<GameObject>) = 0;

    /// <summary>
    /// Factoryを登録します。同じ名前のFactoryが既にある場合は上書きします。
    /// </summary>
    /// <param name="factory">登録するFactory</param>
    static void Register(const std::shared_ptr<IComponentFactory> &factory);

    /// <summary>
    /// 名前に対応するFactoryを取得します。登録されていない場合は例外を投げます。
    /// </summary>
    /// <param name="name">Componentのクラス名(typeidの名前)</param>
    static std::shared_ptr<IComponentFactory> Get(const std::string &name);

    /// <summary>
    /// 登録されているすべてのFactoryの名前を取得します。
    /// </summary>
    static std::vector<std::string> GetNames();
};

/// <summary>
/// T型のComponentをGameObjectに追加するFactoryです。
/// </summary>
template <class T>
class ComponentFactory final : public IComponentFactory
{
public:
    /// <summary>
    /// T のtypeidの名前と、そこから取り出した型名を表示用の名前としてFactoryを作成します。
    /// </summary>
    /// <param name="category">Componentのカテゴリ</param>
    ComponentFactory(const std::string &category = "") : IComponentFactory(typeid(T).name(), EngineUtil::GetTypeName(typeid(T).name()), category)
    { }

    /// <summary>
    /// 指定されたGameObjectに T のComponentを追加します。
    /// </summary>
    /// <param name="game_object">追加先のGameObject</param>
    void AddComponentTo(const std::shared_ptr<GameObject> game_object) override
    {
        game_object->AddComponent<T>();
    }
};

namespace component_factory_util
{
/// <summary>
/// T のComponentFactoryを作成し、登録します。
/// </summary>
/// <param name="category">Componentのカテゴリ</param>
template <class T>
void RegisterComponentFactory(const std::string &category = "")
{
    IComponentFactory::Register(std::make_shared<ComponentFactory<T>>(category));
}
}
}