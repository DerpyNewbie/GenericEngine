#pragma once
#include "CabotEngine/Graphics/ComPtr.h"
#include "CabotEngine/Graphics/Texture2D.h"

#include <directx/d3d12.h>

namespace engine
{
/// <summary>
/// 描画先として使えるTextureです。
/// </summary>
class RenderTexture : public Texture2D
{
    ComPtr<ID3D12DescriptorHeap> m_RTVHeap_;

public:
    /// <summary>
    /// BackBufferと同じ設定のTextureと、RTV用のDescriptorHeapを作成します。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// BufferをRenderTargetとして書き込める状態に遷移させます。Bufferがない場合は作成します。
    /// </summary>
    /// <param name="background_color">背景色(現在は使われていません)</param>
    void BeginRender(Color background_color);
    /// <summary>
    /// BufferをPixelShaderから読める状態に戻します。
    /// </summary>
    void EndRender() const;
    /// <summary>
    /// RTV用のDescriptorHeapを取得します。
    /// </summary>
    ID3D12DescriptorHeap *GetHeap();

    /// <summary>
    /// R8G8B8A8_UNORMのTexture2Dとして読むためのSRVの設定を取得します。
    /// </summary>
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}