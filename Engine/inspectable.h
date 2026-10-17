#pragma once

namespace engine
{
/// <summary>
/// Inspectorに自身のGuiを表示できるもののInterfaceです。
/// </summary>
class Inspectable
{
public:
    virtual ~Inspectable() = default;
    /// <summary>
    /// Inspectorに表示するGuiを描画します。
    /// </summary>
    virtual void OnInspectorGui() = 0;
};
}