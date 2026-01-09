#include "pch.h"
#include "context_menu.h"

namespace editor
{
IContextMenu::IContextMenu(const std::string &target_name)
{
    ContextMenuRegistry::m_menus_.insert_or_assign(target_name, this);
}
}