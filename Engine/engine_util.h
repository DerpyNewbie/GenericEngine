#pragma once

namespace engine
{
class Component;

class EngineUtil
{
public:
    static std::string GetTypeName(const char *typeid_name);
    static std::string GetTypeName(Component *component);
    static std::string GetTypeName(const std::shared_ptr<Component> &component);
    static void ToFloat2(float buff[2], Vector2 vec);
    static void ToFloat3(float buff[3], Vector3 vec);
    static void ToFloat4(float buff[4], Color vec);

    template <typename T>
    static bool OwnerEquals(std::shared_ptr<T> lhs, std::weak_ptr<T> rhs)
    {
        return lhs.owner_before(rhs) && !rhs.owner_before(lhs);
    }

    template <typename Rng, typename T>
    static bool ContainsWeak(Rng rng, const std::shared_ptr<T> &value)
    {
        return std::ranges::find_if(rng, [&](const std::weak_ptr<T> &w) {
            return OwnerEquals(value, w);
        }) != rng.end();
    }
};
}