#pragma once

namespace engine
{
/// <summary>
/// ShaderのResourceとして使えるもののInterfaceです。
/// </summary>
class ShaderResource
{
public:
    virtual ~ShaderResource() = default;

    /// <summary>
    /// ShaderResourceViewの対象となるResourceを取得します。
    /// </summary>
    [[nodiscard]] virtual ID3D12Resource *Resource() = 0;
    /// <summary>
    /// ShaderResourceViewの設定を取得します。
    /// </summary>
    [[nodiscard]] virtual D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() = 0;
};
}