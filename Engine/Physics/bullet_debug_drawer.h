#pragma once
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btVector3.h>

namespace engine
{
/// <summary>
/// Bulletのデバッグ表示をGizmosで描画するクラスです。
/// </summary>
class BulletDebugDrawer : public btIDebugDraw
{
    int m_debug_mode_ = 0;

    /// <summary>
    /// Bulletのデバッグ表示の線をGizmosで描画します。
    /// </summary>
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
    /// <summary>
    /// Bulletのデバッグ表示の接触点を、球と法線方向の線としてGizmosで描画します。
    /// </summary>
    void drawContactPoint(const btVector3 &point_on_b, const btVector3 &normal_on_b, btScalar distance, int life_time,
                          const btVector3 &color) override;
    /// <summary>
    /// Bulletからの警告をエラーとしてログに出力します。
    /// </summary>
    void reportErrorWarning(const char *warning_string) override;
    /// <summary>
    /// 何もしません。文字の描画は未実装です。
    /// </summary>
    void draw3dText(const btVector3 &location, const char *text_string) override;
    /// <summary>
    /// Bulletのデバッグ表示のModeを設定します。
    /// </summary>
    void setDebugMode(int debug_mode) override;
    /// <summary>
    /// Bulletのデバッグ表示のModeを取得します。
    /// </summary>
    int getDebugMode() const override;
};
}