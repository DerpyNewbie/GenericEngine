#pragma once
#include "game_object.h"

namespace editor
{
class AddComponentMenu
{
public:
    static void Draw(const std::shared_ptr<engine::GameObject> &go);
};
}