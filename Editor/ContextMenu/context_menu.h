#pragma once

namespace editor
{
using namespace engine;

/// <summary>
/// 指定されたObjectのContextMenuを表示するInterface。
/// </summary>
class IContextMenu
{
protected:
    /// <summary>
    /// ContextMenuRegistryへの登録を行います。
    /// </summary>
    /// <param name="target_name">対象のObjectのクラス名</param>
    explicit IContextMenu(const std::string &target_name);

public:
    virtual ~IContextMenu() = default;
    /// <summary>
    /// ObjectのContextMenuを表示する
    /// </summary>
    virtual bool OnObjectContextMenu(std::shared_ptr<Object>) = 0;
};

/// <summary>
/// 様々なContextMenuのBaseとなるtemplate class
/// </summary>
template <class T>
class ContextMenu : public IContextMenu
{
public:
    /// <summary>
    /// T のクラス名をキーとしてContextMenuRegistryへの登録を行います。
    /// </summary>
    ContextMenu() : IContextMenu(typeid(T).name()) { }

    /// <summary>
    /// Objectを T にCastし、OnContextMenuを呼び出します。
    /// </summary>
    /// <param name="object">対象のObject</param>
    bool OnObjectContextMenu(const std::shared_ptr<Object> object) override
    {
        return OnContextMenu(std::dynamic_pointer_cast<T>(object));
    }

    /// <summary>
    /// ContextMenuの描画、押された際の実行を行います。
    /// </summary>
    /// <param name="object">対象のObject</param>
    virtual bool OnContextMenu(std::shared_ptr<T> object) = 0;
};

/// <summary>
/// templateで作られたContextMenuを管理するクラス
/// </summary>
class ContextMenuRegistry
{
    friend class IContextMenu;
    inline static std::unordered_map<std::string, IContextMenu *> m_menus_;

public:
    /// <summary>
    /// 指定されたObjectのContextMenuを表示する。
    /// </summary>
    /// <param name="object">表示されるObject</param>
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

    /// <summary>
    /// 指定されたIDのItemが右クリックされた際にContextMenuを表示する。
    /// </summary>
    /// <param name="object">表示されるObject</param>
    /// <param name="id">ItemのID</param>
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
/// <summary>
/// ContextMenuを起動時に登録するためのタグです。
/// </summary>
template <class T>
struct init_ctx_menu
{
};

/// <summary>
/// ContextMenuを起動時に自動で登録するためのクラスです。
/// </summary>
template <class T>
class ContextMenuRegisterer
{
    inline static T m_generated_;

public:
    /// <summary>
    /// m_generated_を参照することで、ContextMenuのインスタンスが静的に生成されるようにします。
    /// </summary>
    /// <returns>自身のコピー</returns>
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