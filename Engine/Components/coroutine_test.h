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
};
}