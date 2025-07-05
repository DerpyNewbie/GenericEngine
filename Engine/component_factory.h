#pragma once
#include "game_object.h"

#include <ranges>

namespace engine
{
class IComponentFactory
{
    friend class Engine;

    static std::unordered_map<std::string, std::shared_ptr<IComponentFactory>> m_factories_;
    std::string m_name_;

    static void Init();

public:
    virtual ~IComponentFactory() = default;

    explicit IComponentFactory(const std::string &name) : m_name_(name)
    {}

    std::string Name()
    {
        return m_name_;
    }

    virtual void AddComponentTo(std::shared_ptr<GameObject>) = 0;

    static void Register(const std::shared_ptr<IComponentFactory> &factory)
    {
        m_factories_.insert_or_assign(factory->Name(), factory);
    }

    static std::shared_ptr<IComponentFactory> Get(const std::string &name)
    {
        return m_factories_.at(name);
    }

    static std::vector<std::string> GetNames()
    {
        auto view = m_factories_ | std::views::keys;
        return {view.begin(), view.end()};
    }
};

template <typename T>
class ComponentFactory final : public IComponentFactory
{
public:
    ComponentFactory() : IComponentFactory(typeid(T).name())
    {}

    void AddComponentTo(const std::shared_ptr<GameObject> game_object) override
    {
        game_object->AddComponent<T>();
    }
};
}