#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/i_command_list.h"
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"
#include "Rendering/CaboRenderingSystem/RHI/shader_reflection.h"

namespace CRS
{
/// <summary>
/// GPUリソースの作成・破棄とフレームの制御を行うInterfaceです。GPUの実体にはこれを経由してしか触りません。
/// </summary>
class IRenderDevice
{
public:
    virtual ~IRenderDevice() = default;

    // ---- リソース作成 ----
    virtual BufferHandle CreateBuffer(const BufferDesc &desc) = 0;
    virtual TextureHandle CreateTexture(const TextureDesc &desc) = 0;
    // HLSLをコンパイルし、リフレクション結果も保持します。失敗した場合は無効なハンドルを返します
    virtual ShaderHandle CreateShader(const ShaderSource &source) = 0;
    // 同じ設定のPSOはキャッシュから返します
    virtual PipelineStateHandle CreatePipelineState(const PipelineStateDesc &desc) = 0;
    virtual SamplerHandle CreateSampler(const SamplerDesc &desc) = 0;

    virtual const ShaderReflection &GetReflection(ShaderHandle shader) const = 0;
    virtual TextureDesc GetTextureDesc(TextureHandle texture) const = 0;

    // ---- 破棄（GPUが使い終わったフレームで解放） ----
    virtual void Destroy(BufferHandle buffer) = 0;
    virtual void Destroy(TextureHandle texture) = 0;
    virtual void Destroy(ShaderHandle shader) = 0;
    virtual void Destroy(PipelineStateHandle pipeline_state) = 0;
    virtual void Destroy(SamplerHandle sampler) = 0;

    // Uploadメモリのバッファに書き込みます。書き込み先は現在のフレームのコピーです
    virtual void UpdateBuffer(BufferHandle buffer, const void *data, uint64_t size, uint64_t offset = 0) = 0;

    // ---- ReadBack（チケット方式） ----
    virtual ReadbackTicket RequestReadback(BufferHandle buffer) = 0;
    virtual bool TryGetReadback(ReadbackTicket ticket, std::vector<uint8_t> &out_data) = 0;

    // ---- コマンド ----
    // そのフレームのコマンドリストを返します。BeginFrameとSubmitの間で使います
    virtual ICommandList &BeginCommandList() = 0;
    virtual void Submit(ICommandList &command_list) = 0;

    // ---- フレーム制御 ----
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;
    virtual TextureHandle GetBackBuffer() const = 0;
    virtual Format GetBackBufferFormat() const = 0;
    virtual uint32_t GetFrameIndex() const = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual void WaitIdle() = 0;
};
}
