#pragma once
#include <directxtk12/Keyboard.h>
#include <directxtk12/Mouse.h>

namespace engine
{
/// <summary>
/// Mouseの動作モードです。
/// </summary>
enum class kMouseMode
{
    kNormal,
    kLocked
};

/// <summary>
/// KeyboardとMouseの入力を管理するクラスです。
/// </summary>
class Input
{
    friend class Application;
    friend class Engine;

    DirectX::Keyboard::State m_keyboard_state_;
    DirectX::Keyboard::KeyboardStateTracker m_keyboard_tracker_;
    std::unique_ptr<DirectX::Keyboard> m_keyboard_;
    std::unique_ptr<DirectX::Mouse> m_mouse_;
    DirectX::Mouse::State m_mouse_state_;
    DirectX::Mouse::ButtonStateTracker m_mouse_tracker_;
    kMouseMode m_mouse_mode_;
    Vector2 m_mouse_position_;
    Vector2 m_mouse_delta_;

    /// <summary>
    /// MouseにWindowを設定し、絶対座標Modeにします。
    /// </summary>
    void Init() const;
    /// <summary>
    /// KeyboardとMouseの状態を更新します。MouseModeがkLockedの時にEscapeが押された場合は、Mouseの固定を切り替えます。
    /// </summary>
    void Update();

public:
    /// <summary>
    /// Inputの唯一のインスタンスを取得します。
    /// </summary>
    static std::shared_ptr<Input> Instance();
    /// <summary>
    /// WindowのMessageをKeyboardとMouseに渡します。
    /// </summary>
    static void ProcessMessage(UINT msg, WPARAM w_param, LPARAM l_param);

    /// <summary>
    /// KeyboardとMouseを生成します。
    /// </summary>
    Input();

    /// <summary>
    /// Keyが押されているかどうかを取得します。
    /// </summary>
    /// <param name="key">対象のKey</param>
    [[nodiscard]] static bool GetKey(DirectX::Keyboard::Keys key);
    /// <summary>
    /// Keyがこのフレームで押されたかどうかを取得します。
    /// </summary>
    /// <param name="key">対象のKey</param>
    [[nodiscard]] static bool GetKeyDown(DirectX::Keyboard::Keys key);
    /// <summary>
    /// Keyがこのフレームで離されたかどうかを取得します。
    /// </summary>
    /// <param name="key">対象のKey</param>
    [[nodiscard]] static bool GetKeyUp(DirectX::Keyboard::Keys key);

    /// <summary>
    /// Mouseの左ボタンが押されているかどうかを取得します。
    /// </summary>
    [[nodiscard]] static bool GetMouseLeft();
    /// <summary>
    /// Mouseの左ボタンがこのフレームで押されたかどうかを取得します。
    /// </summary>
    [[nodiscard]] static bool GetMouseLeftDown();
    /// <summary>
    /// Mouseの左ボタンがこのフレームで離されたかどうかを取得します。
    /// </summary>
    [[nodiscard]] static bool GetMouseLeftUp();
    /// <summary>
    /// Mouseの右ボタンが押されているかどうかを取得します。
    /// </summary>
    [[nodiscard]] static bool GetMouseRight();
    /// <summary>
    /// Mouseの右ボタンがこのフレームで押されたかどうかを取得します。
    /// </summary>
    [[nodiscard]] static bool GetMouseRightDown();
    /// <summary>
    /// Mouseの右ボタンがこのフレームで離されたかどうかを取得します。
    /// </summary>
    [[nodiscard]] static bool GetMouseRightUp();
    /// <summary>
    /// Mouseの座標を取得します。
    /// </summary>
    [[nodiscard]] static Vector2 MousePosition();
    /// <summary>
    /// 前フレームからのMouseの移動量を取得します。
    /// </summary>
    [[nodiscard]] static Vector2 MouseDelta();
    /// <summary>
    /// 現在のMouseModeを取得します。
    /// </summary>
    [[nodiscard]] static kMouseMode MouseMode();

    /// <summary>
    /// MouseModeを設定します。kLockedの場合、Mouseは相対座標Modeになります。
    /// </summary>
    /// <param name="mode">設定するMouseMode</param>
    static void SetMouseMode(kMouseMode mode);
    /// <summary>
    /// Mouseカーソルの表示 / 非表示を設定します。
    /// </summary>
    /// <param name="is_visible">表示する場合 true</param>
    static void SetCursorVisible(bool is_visible);
};
}