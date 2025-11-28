#include "pch.h"
#include "coroutine_test.h"

#include "engine.h"
#include "game_object.h"

namespace engine
{
Task CoroutineTest::Move()
{
    auto transform = GameObject()->Transform();
    while (transform->LocalPosition().x < 100.0f)
    {
        transform->SetLocalPosition(transform->LocalPosition() + Vector3(0.01f, 0, 0));
        co_await WaitForNextFrame();
    }
    Logger::Log<CoroutineTest>("Coroutine has complete!");
}
void CoroutineTest::OnAwake()
{
    Engine::coroutine.Start(Move());
}
}