#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_common.h"

namespace CRS
{
DXGI_FORMAT ToDxgiFormat(const Format format)
{
    switch (format)
    {
    case Format::R8G8B8A8_UNorm:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Format::R8G8B8A8_UNorm_sRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case Format::B8G8R8A8_UNorm:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case Format::R16G16B16A16_Float:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case Format::R32_Float:
        return DXGI_FORMAT_R32_FLOAT;
    case Format::R32G32_Float:
        return DXGI_FORMAT_R32G32_FLOAT;
    case Format::R32G32B32_Float:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case Format::R32G32B32A32_Float:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case Format::R32_UInt:
        return DXGI_FORMAT_R32_UINT;
    case Format::R32G32B32A32_UInt:
        return DXGI_FORMAT_R32G32B32A32_UINT;
    case Format::BC1_UNorm:
        return DXGI_FORMAT_BC1_UNORM;
    case Format::BC2_UNorm:
        return DXGI_FORMAT_BC2_UNORM;
    case Format::BC3_UNorm:
        return DXGI_FORMAT_BC3_UNORM;
    case Format::D32_Float:
        return DXGI_FORMAT_D32_FLOAT;
    case Format::Unknown:
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

Format FromDxgiFormat(const DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return Format::R8G8B8A8_UNorm;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return Format::R8G8B8A8_UNorm_sRGB;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return Format::B8G8R8A8_UNorm;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return Format::R16G16B16A16_Float;
    case DXGI_FORMAT_R32_FLOAT:
        return Format::R32_Float;
    case DXGI_FORMAT_R32G32_FLOAT:
        return Format::R32G32_Float;
    case DXGI_FORMAT_R32G32B32_FLOAT:
        return Format::R32G32B32_Float;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return Format::R32G32B32A32_Float;
    case DXGI_FORMAT_R32_UINT:
        return Format::R32_UInt;
    case DXGI_FORMAT_R32G32B32A32_UINT:
        return Format::R32G32B32A32_UInt;
    case DXGI_FORMAT_BC1_UNORM:
        return Format::BC1_UNorm;
    case DXGI_FORMAT_BC2_UNORM:
        return Format::BC2_UNorm;
    case DXGI_FORMAT_BC3_UNORM:
        return Format::BC3_UNorm;
    case DXGI_FORMAT_D32_FLOAT:
        return Format::D32_Float;
    default:
        return Format::Unknown;
    }
}

D3D12_RESOURCE_STATES ToD3D12State(const ResourceState state)
{
    switch (state)
    {
    case ResourceState::VertexAndConstantBuffer:
        return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    case ResourceState::IndexBuffer:
        return D3D12_RESOURCE_STATE_INDEX_BUFFER;
    case ResourceState::RenderTarget:
        return D3D12_RESOURCE_STATE_RENDER_TARGET;
    case ResourceState::DepthWrite:
        return D3D12_RESOURCE_STATE_DEPTH_WRITE;
    case ResourceState::DepthRead:
        return D3D12_RESOURCE_STATE_DEPTH_READ;
    case ResourceState::ShaderResource:
        return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
    case ResourceState::UnorderedAccess:
        return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    case ResourceState::CopySource:
        return D3D12_RESOURCE_STATE_COPY_SOURCE;
    case ResourceState::CopyDest:
        return D3D12_RESOURCE_STATE_COPY_DEST;
    case ResourceState::Present:
        return D3D12_RESOURCE_STATE_PRESENT;
    case ResourceState::GenericRead:
        return D3D12_RESOURCE_STATE_GENERIC_READ;
    case ResourceState::Common:
    default:
        return D3D12_RESOURCE_STATE_COMMON;
    }
}

D3D12_COMPARISON_FUNC ToD3D12Compare(const CompareOp op)
{
    switch (op)
    {
    case CompareOp::Never:
        return D3D12_COMPARISON_FUNC_NEVER;
    case CompareOp::Less:
        return D3D12_COMPARISON_FUNC_LESS;
    case CompareOp::LessEqual:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case CompareOp::Equal:
        return D3D12_COMPARISON_FUNC_EQUAL;
    case CompareOp::Greater:
        return D3D12_COMPARISON_FUNC_GREATER;
    case CompareOp::GreaterEqual:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case CompareOp::NotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case CompareOp::Always:
    default:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    }
}

D3D12_CULL_MODE ToD3D12Cull(const CullMode mode)
{
    switch (mode)
    {
    case CullMode::None:
        return D3D12_CULL_MODE_NONE;
    case CullMode::Front:
        return D3D12_CULL_MODE_FRONT;
    case CullMode::Back:
    default:
        return D3D12_CULL_MODE_BACK;
    }
}

D3D12_BLEND_DESC ToD3D12Blend(const BlendMode mode)
{
    auto desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    auto &rt = desc.RenderTarget[0];
    switch (mode)
    {
    case BlendMode::AlphaBlend:
        rt.BlendEnable = TRUE;
        rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        rt.BlendOp = D3D12_BLEND_OP_ADD;
        rt.SrcBlendAlpha = D3D12_BLEND_ONE;
        rt.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
        rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        break;
    case BlendMode::Additive:
        rt.BlendEnable = TRUE;
        rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        rt.DestBlend = D3D12_BLEND_ONE;
        rt.BlendOp = D3D12_BLEND_OP_ADD;
        rt.SrcBlendAlpha = D3D12_BLEND_ZERO;
        rt.DestBlendAlpha = D3D12_BLEND_ONE;
        rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        break;
    case BlendMode::Opaque:
    default:
        break;
    }
    return desc;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12TopologyType(const PrimitiveTopology topology)
{
    return topology == PrimitiveTopology::LineList
               ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE
               : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

D3D12_PRIMITIVE_TOPOLOGY ToD3D12Topology(const PrimitiveTopology topology)
{
    return topology == PrimitiveTopology::LineList
               ? D3D_PRIMITIVE_TOPOLOGY_LINELIST
               : D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

D3D12_FILTER ToD3D12Filter(const Filter filter)
{
    switch (filter)
    {
    case Filter::Point:
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    case Filter::Anisotropic:
        return D3D12_FILTER_ANISOTROPIC;
    case Filter::Linear:
    default:
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    }
}

D3D12_TEXTURE_ADDRESS_MODE ToD3D12Address(const AddressMode mode)
{
    switch (mode)
    {
    case AddressMode::Clamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case AddressMode::Mirror:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case AddressMode::Wrap:
    default:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }
}
}