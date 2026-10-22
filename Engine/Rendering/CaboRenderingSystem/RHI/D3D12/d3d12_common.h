#pragma once
// D3D12の型を知ってよいのは RHI/D3D12 の中だけです。
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wrl/client.h>
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

namespace CRS
{
template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

DXGI_FORMAT ToDxgiFormat(Format format);
Format FromDxgiFormat(DXGI_FORMAT format);
D3D12_RESOURCE_STATES ToD3D12State(ResourceState state);
D3D12_COMPARISON_FUNC ToD3D12Compare(CompareOp op);
D3D12_CULL_MODE ToD3D12Cull(CullMode mode);
D3D12_BLEND_DESC ToD3D12Blend(BlendMode mode);
D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12TopologyType(PrimitiveTopology topology);
D3D12_PRIMITIVE_TOPOLOGY ToD3D12Topology(PrimitiveTopology topology);
D3D12_FILTER ToD3D12Filter(Filter filter);
D3D12_TEXTURE_ADDRESS_MODE ToD3D12Address(AddressMode mode);
}