#pragma once
#include "Rendering/CaboRenderingSystem/RHI/i_command_list.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_common.h"

namespace CRS
{
class D3D12RenderDevice;

/// <summary>
/// ICommandListのD3D12実装です。ルートシグネチャとディスクリプタヒープの設定はこの中に隠します。
/// </summary>
class D3D12CommandList final : public ICommandList
{
    D3D12RenderDevice &m_device_;
    ComPtr<ID3D12GraphicsCommandList> m_list_;

    PipelineStateHandle m_current_pipeline_state_;
    PrimitiveTopology m_current_topology_ = PrimitiveTopology::TriangleList;
    bool m_root_state_bound_ = false;

    void BindRootState();

public:
    D3D12CommandList(D3D12RenderDevice &device, ComPtr<ID3D12GraphicsCommandList> list);

    ID3D12GraphicsCommandList *Native() const
    {
        return m_list_.Get();
    }

    /// <summary>
    /// キャッシュしている設定を捨てます。外部（旧描画システム）が同じコマンドリストに記録した後に呼びます。
    /// </summary>
    void InvalidateState();

    void Transition(TextureHandle texture, ResourceState state) override;
    void Transition(BufferHandle buffer, ResourceState state) override;

    void BeginRenderPass(const RenderPassDesc &desc) override;
    void EndRenderPass() override;

    void SetViewport(const Viewport &viewport) override;
    void SetScissor(const ScissorRect &scissor) override;

    void SetPipelineState(PipelineStateHandle pipeline_state) override;
    void SetBindings(const Bindings &bindings) override;

    void SetVertexBuffer(BufferHandle buffer, uint32_t stride, uint64_t offset = 0) override;
    void SetVertexData(const TransientVertices &vertices) override;
    void SetIndexBuffer(BufferHandle buffer, uint64_t offset = 0) override;

    void Draw(uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0,
              uint32_t first_instance = 0) override;
    void DrawIndexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0,
                     int32_t base_vertex = 0, uint32_t first_instance = 0) override;

    void CopyBuffer(BufferHandle dst, uint64_t dst_offset, BufferHandle src, uint64_t src_offset,
                    uint64_t size) override;

    void BeginDebugEvent(const char *name) override;
    void EndDebugEvent() override;
};
}