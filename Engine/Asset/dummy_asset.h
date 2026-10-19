#pragma once

namespace engine
{
/// <summary>
/// 動作確認用の、中身を持たないAssetです。
/// </summary>
class DummyAsset : public Object, public Inspectable
{
    void OnInspectorGui() override;
};
}