#pragma once
#include "game_object.h"

namespace engine
{
class IComponentFactory
{
    friend class Engine;

    static std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> m_factories_;
    std::string m_name_;
    std::string m_friendly_name_;
    std::string m_category_;

    static void Init();

public:
    virtual ~IComponentFactory() = default;

    explicit IComponentFactory(const std::string &name, const std::string &friendly_name, const std::string &category);

    std::string Name();
    std::string FriendlyName();
    std::string Category();

    virtual void AddComponentTo(std::shared_ptr<GameObject>) = 0;

    static void Register(const std::shared_ptr<IComponentFactory> &factory);

    static std::shared_ptr<IComponentFactory> Get(const std::string &name);

    static std::vector<std::string> GetNames();
};

template <class T>
class ComponentFactory final : public IComponentFactory
{
public:
    ComponentFactory(const std::string &category = "") : IComponentFactory(typeid(T).name(), EngineUtil::GetTypeName(typeid(T).name()), category)
    { }

    void AddComponentTo(const std::shared_ptr<GameObject> game_object) override
    {
        game_object->AddComponent<T>();
    }
};

namespace component_factory_util
{
template <class T>
void RegisterComponentFactory(const std::string &category = "")
{
    IComponentFactory::Register(std::make_shared<ComponentFactory<T>>(category));
}
}
}