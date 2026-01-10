#pragma once
#include "game_object.h"

namespace editor
{
using namespace engine;
class GameObjectCreateMenu
{
public:
    static void Draw(const std::shared_ptr<GameObject> &go);
};
}