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

// ---- RHIの型とD3D12 / DXGIの型の変換（d3d12_convert.cpp） ----

/// <summary>
/// Formatを対応するDXGI_FORMATに変換します。
/// </summary>
DXGI_FORMAT ToDxgiFormat(Format format);
/// <summary>
/// DXGI_FORMATをFormatに変換します。対応していないフォーマットはUnknownになります（外部のテクスチャを取り込むときに使います）。
/// </summary>
Format FromDxgiFormat(DXGI_FORMAT format);
/// <summary>
/// ResourceStateを対応するD3D12_RESOURCE_STATESに変換します。
/// </summary>
D3D12_RESOURCE_STATES ToD3D12State(ResourceState state);
/// <summary>
/// CompareOpを対応するD3D12_COMPARISON_FUNCに変換します。
/// </summary>
D3D12_COMPARISON_FUNC ToD3D12Compare(CompareOp op);
/// <summary>
/// CullModeを対応するD3D12_CULL_MODEに変換します。
/// </summary>
D3D12_CULL_MODE ToD3D12Cull(CullMode mode);
/// <summary>
/// BlendModeから、描画先0のブレンド設定を作ります。
/// </summary>
D3D12_BLEND_DESC ToD3D12Blend(BlendMode mode);
/// <summary>
/// PSOに設定するトポロジの種類（三角形 / 線）に変換します。
/// </summary>
D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12TopologyType(PrimitiveTopology topology);
/// <summary>
/// コマンドリストに設定するトポロジ（TriangleList / LineList）に変換します。
/// </summary>
D3D12_PRIMITIVE_TOPOLOGY ToD3D12Topology(PrimitiveTopology topology);
/// <summary>
/// Filterを対応するD3D12_FILTERに変換します。
/// </summary>
D3D12_FILTER ToD3D12Filter(Filter filter);
/// <summary>
/// AddressModeを対応するD3D12_TEXTURE_ADDRESS_MODEに変換します。
/// </summary>
D3D12_TEXTURE_ADDRESS_MODE ToD3D12Address(AddressMode mode);
}
