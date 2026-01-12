#pragma once
#include "../context_menu.h"
#include "Components/component.h"

namespace editor
{
class ComponentContextMenu : public ContextMenu<Component>
{
    bool OnContextMenu(std::shared_ptr<Component> component) override;
};
}