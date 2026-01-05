#include "pch.h"
#include "coroutine_test.h"

#include "engine.h"
#include "game_object.h"
#include "do.h"

namespace engine
{
Task CoroutineTest::Move()
{
    auto transform = GameObject()->Transform();
    while (transform->LocalPosition().x < 100.0f)
    {
        transform->SetLocalPosition(transform->LocalPosition() + Vector3(0.1f, 0, 0));
        co_await WaitForNextFrame();
    }
    co_return;
}

Task CoroutineTest::MoveWrap()
{
    co_await WaitForTask(Move());
    Logger::Log<CoroutineTest>("Coroutine has complete!");
    co_return;
}

void CoroutineTest::OnAwake()
{
    Engine::coroutine.Start(Do::Move(GameObject()->Transform(), Vector3(100, 0, 0), 10.0f));
}
}

CEREAL_REGISTER_TYPE(engine::CoroutineTest)