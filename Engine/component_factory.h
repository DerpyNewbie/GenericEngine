#pragma once
#include "game_object.h"

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

    explicit IComponentFactory(const std::string &name);

    std::string Name();

    virtual void AddComponentTo(std::shared_ptr<GameObject>) = 0;

    static void Register(const std::shared_ptr<IComponentFactory> &factory);

    static std::shared_ptr<IComponentFactory> Get(const std::string &name);

    static std::vector<std::string> GetNames();
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