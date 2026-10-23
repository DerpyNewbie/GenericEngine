#include "pch.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_command_list.h"

#include <cstring>

#include "Rendering/CaboRenderingSystem/Core/log.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_render_device.h"

namespace CRS
{
D3D12CommandList::D3D12CommandList(D3D12RenderDevice &device, ComPtr<ID3D12GraphicsCommandList> list) :
    m_device_(device), m_list_(std::move(list))
{}

void D3D12CommandList::InvalidateState()
{
    m_current_pipeline_state_ = {};
    m_root_state_bound_ = false;
}

void D3D12CommandList::BindRootState()
{
    if (m_root_state_bound_)
        return;

    ID3D12DescriptorHeap *heaps[] = {m_device_.m_gpu_srv_ring_.Heap(), m_device_.m_gpu_sampler_ring_.Heap()};
    m_list_->SetDescriptorHeaps(2, heaps);
    m_list_->SetGraphicsRootSignature(m_device_.m_root_signature_.Get());
    m_root_state_bound_ = true;
}

void D3D12CommandList::Transition(const TextureHandle texture_handle, const ResourceState state)
{
    const auto texture = m_device_.m_textures_.Get(texture_handle);
    if (texture == nullptr)
    {
        LogError("Transition: invalid texture handle");
        return;
    }

    if (texture->state == state)
        return;

    if (texture->fixed_state)
    {
        // 外部から取り込んだテクスチャの状態は変えない
        return;
    }

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture->resource.Get(), ToD3D12State(texture->state),
                                                              ToD3D12State(state));
    m_list_->ResourceBarrier(1, &barrier);
    texture->state = state;
}

void D3D12CommandList::Transition(const BufferHandle buffer_handle, const ResourceState state)
{
    const auto buffer = m_device_.m_buffers_.Get(buffer_handle);
    if (buffer == nullptr)
    {
        LogError("Transition: invalid buffer handle");
        return;
    }

    // Upload / Readbackのバッファは状態を変えられない
    if (buffer->desc.memory != MemoryType::GpuOnly || buffer->state == state)
        return;

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer->resources[0].Get(),
                                                              ToD3D12State(buffer->state), ToD3D12State(state));
    m_list_->ResourceBarrier(1, &barrier);
    buffer->state = state;
}

void D3D12CommandList::BeginRenderPass(const RenderPassDesc &desc)
{
    // 外部が同じコマンドリストに記録している可能性があるので、設定し直す
    InvalidateState();
    BindRootState();

    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kMaxRenderTargets> rtvs = {};
    for (uint32_t i = 0; i < desc.color_target_count; ++i)
    {
        rtvs[i] = m_device_.GetRtv(desc.color_targets[i]);
        if (desc.color_load == LoadOp::Clear)
            m_list_->ClearRenderTargetView(rtvs[i], desc.clear_color.data(), 0, nullptr);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
    const bool has_depth = desc.depth_target.IsValid();
    if (has_depth)
    {
        dsv = m_device_.GetDsv(desc.depth_target, desc.depth_slice);
        if (desc.depth_load == LoadOp::Clear)
            m_list_->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, desc.clear_depth, 0, 0, nullptr);
    }

    m_list_->OMSetRenderTargets(desc.color_target_count, desc.color_target_count > 0 ? rtvs.data() : nullptr, FALSE,
                                has_depth ? &dsv : nullptr);
}

void D3D12CommandList::EndRenderPass()
{
    // 当面はやることなし。フレームグラフを入れるときにバリアの解決をここで行う
}

void D3D12CommandList::SetViewport(const Viewport &viewport)
{
    const D3D12_VIEWPORT native = {
        viewport.x, viewport.y, viewport.width, viewport.height, viewport.min_depth, viewport.max_depth
    };
    m_list_->RSSetViewports(1, &native);
}

void D3D12CommandList::SetScissor(const ScissorRect &scissor)
{
    const D3D12_RECT native = {scissor.left, scissor.top, scissor.right, scissor.bottom};
    m_list_->RSSetScissorRects(1, &native);
}

void D3D12CommandList::SetPipelineState(const PipelineStateHandle pipeline_state_handle)
{
    BindRootState();

    if (m_current_pipeline_state_ == pipeline_state_handle)
        return;

    const auto pipeline_state = m_device_.m_pipeline_states_.Get(pipeline_state_handle);
    if (pipeline_state == nullptr)
    {
        LogError("SetPipelineState: invalid pipeline state handle");
        return;
    }

    m_list_->SetPipelineState(pipeline_state->pipeline_state.Get());
    m_list_->IASetPrimitiveTopology(ToD3D12Topology(pipeline_state->desc.topology));
    m_current_pipeline_state_ = pipeline_state_handle;
}

void D3D12CommandList::SetBindings(const Bindings &bindings)
{
    BindRootState();

    // 定数: CPUの値はアップロード用のリングにコピーし、GPUバッファはそのアドレスを使う
    // 何も渡されていない場合も、シェーダーが読む可能性があるので0で埋めた領域を渡す（1回の呼び出しの中で共有）
    D3D12_GPU_VIRTUAL_ADDRESS zero_address = 0;
    const auto bind_constant = [&](const ConstantBinding &constant, const uint32_t parameter) {
        D3D12_GPU_VIRTUAL_ADDRESS address = 0;
        if (constant.buffer.IsValid())
        {
            address = m_device_.GetBufferAddress(constant.buffer);
        }
        else if (constant.data != nullptr)
        {
            uint8_t *cpu = nullptr;
            if (m_device_.AllocateUpload(constant.size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, cpu, address))
                std::memcpy(cpu, constant.data, constant.size);
        }
        else
        {
            uint8_t *cpu = nullptr;
            if (zero_address == 0 &&
                m_device_.AllocateUpload(256, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, cpu, zero_address))
                std::memset(cpu, 0, 256);
            address = zero_address;
        }

        if (address != 0)
            m_list_->SetGraphicsRootConstantBufferView(parameter, address);
    };
    for (uint32_t slot = 0; slot < kConstantSlotCount; ++slot)
        bind_constant(bindings.constants[slot], kRootParameter_Constants + slot);
    for (uint32_t slot = 0; slot < kFrameConstantSlotCount; ++slot)
        bind_constant(bindings.frame_constants[slot], kRootParameter_FrameConstants + slot);

    // t0..t7: リソースごとのSRVを、シェーダーから見えるリングに並べてコピーする
    const auto native_device = m_device_.m_device_.Get();
    const auto bind_table = [&](const ResourceBinding *resources, const uint32_t count, const uint32_t parameter) {
        GpuDescriptorRange range;
        if (!m_device_.m_gpu_srv_ring_.Allocate(count, range))
            return;

        for (uint32_t slot = 0; slot < count; ++slot)
        {
            native_device->CopyDescriptorsSimple(1, range.Cpu(slot), m_device_.GetResourceSrv(resources[slot]),
                                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
        m_list_->SetGraphicsRootDescriptorTable(parameter, range.gpu);
    };
    bind_table(bindings.resources.data(), kResourceSlotCount, kRootParameter_Resources);
    bind_table(bindings.frame_resources.data(), kFrameResourceSlotCount, kRootParameter_FrameResources);

    // s0..s3
    GpuDescriptorRange sampler_range;
    if (m_device_.m_gpu_sampler_ring_.Allocate(kSamplerSlotCount, sampler_range))
    {
        for (uint32_t slot = 0; slot < kSamplerSlotCount; ++slot)
        {
            native_device->CopyDescriptorsSimple(1, sampler_range.Cpu(slot),
                                                 m_device_.GetSamplerDescriptor(bindings.samplers[slot]),
                                                 D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        }
        m_list_->SetGraphicsRootDescriptorTable(kRootParameter_Samplers, sampler_range.gpu);
    }
}

void D3D12CommandList::SetVertexBuffer(const BufferHandle buffer_handle, const uint32_t stride, const uint64_t offset)
{
    const auto buffer = m_device_.m_buffers_.Get(buffer_handle);
    if (buffer == nullptr)
    {
        LogError("SetVertexBuffer: invalid buffer handle");
        return;
    }

    D3D12_VERTEX_BUFFER_VIEW view;
    view.BufferLocation = m_device_.GetBufferAddress(buffer_handle) + offset;
    view.SizeInBytes = static_cast<UINT>(buffer->desc.size - offset);
    view.StrideInBytes = stride;
    m_list_->IASetVertexBuffers(0, 1, &view);
}

void D3D12CommandList::SetVertexData(const TransientVertices &vertices)
{
    if (vertices.data == nullptr || vertices.size == 0)
        return;

    uint8_t *cpu = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS address = 0;
    if (!m_device_.AllocateUpload(vertices.size, 16, cpu, address))
        return;

    std::memcpy(cpu, vertices.data, vertices.size);

    D3D12_VERTEX_BUFFER_VIEW view;
    view.BufferLocation = address;
    view.SizeInBytes = vertices.size;
    view.StrideInBytes = vertices.stride;
    m_list_->IASetVertexBuffers(0, 1, &view);
}

void D3D12CommandList::SetIndexBuffer(const BufferHandle buffer_handle, const uint64_t offset)
{
    const auto buffer = m_device_.m_buffers_.Get(buffer_handle);
    if (buffer == nullptr)
    {
        LogError("SetIndexBuffer: invalid buffer handle");
        return;
    }

    D3D12_INDEX_BUFFER_VIEW view;
    view.BufferLocation = m_device_.GetBufferAddress(buffer_handle) + offset;
    view.SizeInBytes = static_cast<UINT>(buffer->desc.size - offset);
    view.Format = DXGI_FORMAT_R32_UINT;
    m_list_->IASetIndexBuffer(&view);
}

void D3D12CommandList::Draw(const uint32_t vertex_count, const uint32_t instance_count, const uint32_t first_vertex,
                            const uint32_t first_instance)
{
    m_list_->DrawInstanced(vertex_count, instance_count, first_vertex, first_instance);
}

void D3D12CommandList::DrawIndexed(const uint32_t index_count, const uint32_t instance_count,
                                   const uint32_t first_index, const int32_t base_vertex,
                                   const uint32_t first_instance)
{
    m_list_->DrawIndexedInstanced(index_count, instance_count, first_index, base_vertex, first_instance);
}

void D3D12CommandList::CopyBuffer(const BufferHandle dst, const uint64_t dst_offset, const BufferHandle src,
                                  const uint64_t src_offset, const uint64_t size)
{
    const auto dst_buffer = m_device_.m_buffers_.Get(dst);
    const auto src_buffer = m_device_.m_buffers_.Get(src);
    if (dst_buffer == nullptr || src_buffer == nullptr)
    {
        LogError("CopyBuffer: invalid buffer handle");
        return;
    }

    if (dst_buffer->IsVersioned())
    {
        LogError("CopyBuffer: cannot copy into an Upload buffer");
        return;
    }

    const auto src_version = src_buffer->IsVersioned() ? m_device_.m_frame_index_ : 0;
    m_list_->CopyBufferRegion(dst_buffer->resources[0].Get(), dst_offset, src_buffer->resources[src_version].Get(),
                              src_offset, size);
}

void D3D12CommandList::BeginDebugEvent(const char *name)
{
    // PIXのマーカー形式（ANSI文字列）
    m_list_->BeginEvent(1, name, static_cast<UINT>(std::strlen(name) + 1));
}

void D3D12CommandList::EndDebugEvent()
{
    m_list_->EndEvent();
}
}
