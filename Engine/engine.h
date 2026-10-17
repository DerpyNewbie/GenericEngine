#pragma once
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd")
#else
#pragma comment(lib, "assimp-vc143-mt")
#endif
#include "event.h"
#include "Coroutine/coroutine_manager.h"

namespace engine
{
/// <summary>
/// Engine全体の初期化、毎フレームの更新、終了処理を行うクラスです。
/// </summary>
class Engine
{
public:
    static Event<> on_init;
    static Event<> on_default_scene_creation;
    static Event<> on_tick;
    static Event<> on_finalize;
    inline static CoroutineManager coroutine;

    /// <summary>
    /// RenderEngineなど各Systemを初期化し、on_initとon_default_scene_creationを呼び出します。
    /// </summary>
    /// <returns>常に true</returns>
    static bool Init();
    /// <summary>
    /// 1フレーム分の処理(入力の更新、FixedUpdate、Update、Coroutine、描画、Objectの破棄)を行います。
    /// </summary>
    static void Tick();
    /// <summary>
    /// on_finalizeを呼び出します。
    /// </summary>
    static void Finalize();
};
}