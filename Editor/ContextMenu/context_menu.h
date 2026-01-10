#pragma once

namespace editor
{
using namespace engine;

class IContextMenu
{
protected:
    explicit IContextMenu(const std::string &target_name);

public:
    virtual ~IContextMenu() = default;
    virtual bool OnObjectContextMenu(std::shared_ptr<Object>) = 0;
};

template <class T>
class ContextMenu : public IContextMenu
{
public:
    ContextMenu() : IContextMenu(typeid(T).name()) { }

    bool OnObjectContextMenu(const std::shared_ptr<Object> object) override
    {
        return OnContextMenu(std::dynamic_pointer_cast<T>(object));
    }

    virtual bool OnContextMenu(std::shared_ptr<T> object) = 0;
};

class ContextMenuRegistry
{
    friend class IContextMenu;
    inline static std::unordered_map<std::string, IContextMenu *> m_menus_;

public:
    template <class T>
    static void DrawMenuInline(std::shared_ptr<T> object)
    {
        const auto pos = m_menus_.find(typeid(T).name());
        if (pos == m_menus_.end())
        {
            ImGui::LabelText("No context menu is found for %s", object->Name().c_str());
            return;
        }

        pos->second->OnObjectContextMenu(object);
    }

    template <class T>
    static void DrawPopup(std::shared_ptr<T> object, const char *id = nullptr)
    {
        if (ImGui::BeginPopupContextItem(id))
        {
            DrawMenuInline(object);
            ImGui::EndPopup();
        }
    }
};

namespace generated_internals::context_menu
{
template <class T>
struct init_ctx_menu
{
};

template <class T>
class ContextMenuRegisterer
{
    inline static T m_generated_;

public:
    ContextMenuRegisterer Bind()
    {
        (void)m_generated_;
        return *this;
    }
};
}
}

#define REGISTER_CONTEXT_MENU(T) \
namespace editor::generated_internals::context_menu {\
template<>\
struct init_ctx_menu<##T>\
{\
static inline ContextMenuRegisterer<##T> const &b = cereal::detail::StaticObject<ContextMenuRegisterer<##T>>::getInstance().Bind();\
static void unused() { (void)b; }\
};\
}