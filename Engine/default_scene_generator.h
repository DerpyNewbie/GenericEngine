#pragma once

namespace engine
{
/// <summary>
/// サンプル用のデフォルトSceneを作成するクラスです。
/// </summary>
class SampleSceneGenerator
{
public:
    /// <summary>
    /// "Default Scene"を作成し、Camera、床、Y Bot、RenderingSettingsを配置します。
    /// </summary>
    static void CreateDefaultScene();
    /// <summary>
    /// CameraComponentとAudioListenerComponentを持つ"Camera"を作成します。
    /// </summary>
    static void CreateDefaultCamera();
    /// <summary>
    /// PlaneColliderを持つ"Floor"と、床として表示するCubeのModelを作成します。
    /// </summary>
    static void CreateDefaultFloor();
    /// <summary>
    /// hackadollのModelをFBXから読み込み、1/100のScaleで配置します。
    /// </summary>
    static void CreateHackadoll();
    /// <summary>
    /// Y BotのModelをFBXから読み込み、1/100のScaleで配置します。
    /// </summary>
    static void CreateYBot();
    /// <summary>
    /// RenderingSettingsComponentを持つ"RenderingSettings"を作成します。
    /// </summary>
    static void CreateRenderingSettings();
};
}