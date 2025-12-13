#pragma once
#include "component.h"
#include "Coroutine/task.h"

namespace engine
{
class CoroutineTest : public Component
{
    Task Move();

public:
    void OnAwake() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::CoroutineTest, 1)