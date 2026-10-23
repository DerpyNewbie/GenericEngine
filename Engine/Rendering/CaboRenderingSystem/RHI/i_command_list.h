#pragma once
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

namespace CRS
{
/// <summary>
/// 描画コマンドを記録するInterfaceです。当面はフレームに1本です（IRenderDevice::BeginCommandListで取得します）。
/// </summary>
class ICommandList
{
public:
    virtual ~ICommandList() = default;

    /// <summary>
    /// テクスチャの状態を切り替えます（描画先にする前、シェーダーで読む前など）。
    /// 今の状態と同じ場合と、外部から取り込んだ状態固定のテクスチャの場合は何もしません。
    /// </summary>
    virtual void Transition(TextureHandle texture, ResourceState state) = 0;
    /// <summary>
    /// バッファの状態を切り替えます。今の状態と同じ場合と、Upload / Readbackのバッファ（状態が固定）の場合は何もしません。
    /// </summary>
    virtual void Transition(BufferHandle buffer, ResourceState state) = 0;

    /// <summary>
    /// 描画先を設定し、必要ならクリアします。描画先のTransitionは先に済ませておきます。
    /// </summary>
    virtual void BeginRenderPass(const RenderPassDesc &desc) = 0;
    /// <summary>
    /// BeginRenderPassで始めた描画を終えます。今は何もしませんが、将来の状態の自動解決のために対にして呼んでください。
    /// </summary>
    virtual void EndRenderPass() = 0;

    /// <summary>
    /// 描画する範囲を設定します。
    /// </summary>
    virtual void SetViewport(const Viewport &viewport) = 0;
    /// <summary>
    /// 描画を切り抜く範囲を設定します。
    /// </summary>
    virtual void SetScissor(const ScissorRect &scissor) = 0;

    /// <summary>
    /// PSOを設定します。トポロジ（三角形 / 線）もPSOの設定から決まります。
    /// </summary>
    virtual void SetPipelineState(PipelineStateHandle pipeline_state) = 0;
    /// <summary>
    /// 定数・テクスチャ・サンプラーをまとめて設定します。CPUの定数とディスクリプタは、ここでこのフレーム用のリングにコピーされます。
    /// </summary>
    virtual void SetBindings(const Bindings &bindings) = 0;

    /// <summary>
    /// GPUにある頂点バッファを設定します。
    /// </summary>
    virtual void SetVertexBuffer(BufferHandle buffer, uint32_t stride, uint64_t offset = 0) = 0;
    /// <summary>
    /// CPUの頂点データをその場でアップロードして使います（線やUIなど、毎フレーム作り直すもの）。
    /// </summary>
    virtual void SetVertexData(const TransientVertices &vertices) = 0;
    /// <summary>
    /// インデックスバッファを設定します。インデックスは32bit固定です。
    /// </summary>
    virtual void SetIndexBuffer(BufferHandle buffer, uint64_t offset = 0) = 0;

    /// <summary>
    /// インデックスなしで描画します。
    /// </summary>
    virtual void Draw(uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0,
                      uint32_t first_instance = 0) = 0;
    /// <summary>
    /// インデックスバッファを使って描画します。
    /// </summary>
    /// <param name="base_vertex">インデックスの値に足す頂点番号（サブメッシュが頂点バッファの途中から始まる場合など）</param>
    virtual void DrawIndexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0,
                             int32_t base_vertex = 0, uint32_t first_instance = 0) = 0;

    /// <summary>
    /// GPU上でバッファの一部をコピーします。状態の切り替え（CopyDest / CopySource）は呼び出し側で先に行います。
    /// コピー先はGpuOnlyのバッファだけです。コピー元がUploadなら今のフレームのコピーを使います。
    /// </summary>
    virtual void CopyBuffer(BufferHandle dst, uint64_t dst_offset, BufferHandle src, uint64_t src_offset,
                            uint64_t size) = 0;

    /// <summary>
    /// PIXなどのデバッガーで見える区切りを始めます。EndDebugEventと対にします。
    /// </summary>
    virtual void BeginDebugEvent(const char *name) = 0;
    /// <summary>
    /// BeginDebugEventで始めた区切りを終えます。
    /// </summary>
    virtual void EndDebugEvent() = 0;
};
}
