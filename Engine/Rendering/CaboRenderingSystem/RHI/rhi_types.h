#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
constexpr uint32_t kMaxFramesInFlight = 2;
constexpr uint32_t kMaxRenderTargets = 8;
// 共通ルートシグネチャのスロット数です
//   space0（マテリアル）: b0..b3 / t0..t7 / s0..s3。シェーダー側はregisterを書かなくてよい（DXCが順に割り当てる）
//   space1（エンジン）  : b0 View / b1 Object / b2 Lighting / t0..t7 ライト・影・ボーン
//                         固定のサンプラー（s0: 影の比較用, s1: 線形Clamp）
constexpr uint32_t kConstantSlotCount = 4;
constexpr uint32_t kFrameConstantSlotCount = 3;
constexpr uint32_t kResourceSlotCount = 8;
constexpr uint32_t kFrameResourceSlotCount = 8;
constexpr uint32_t kSamplerSlotCount = 4;
// 配列テクスチャで、全スライスを指す場合のスライス番号
constexpr uint32_t kAllSlices = UINT32_MAX;

/// <summary>
/// ピクセル / 頂点要素のフォーマットです。DXGI_FORMATの代わりに使います。
/// </summary>
enum class Format : uint8_t
{
    Unknown,
    R8G8B8A8_UNorm,
    R8G8B8A8_UNorm_sRGB,
    B8G8R8A8_UNorm,
    R16G16B16A16_Float,
    R32_Float,
    R32G32_Float,
    R32G32B32_Float,
    R32G32B32A32_Float,
    R32_UInt,
    R32G32B32A32_UInt,
    D32_Float,
    BC1_UNorm,
    BC2_UNorm,
    BC3_UNorm,
};

/// <summary>
/// ブロック圧縮のフォーマットかどうかです。
/// </summary>
constexpr bool IsBlockCompressed(const Format format)
{
    return format == Format::BC1_UNorm || format == Format::BC2_UNorm || format == Format::BC3_UNorm;
}

/// <summary>
/// リソースの状態です。D3D12_RESOURCE_STATESの代わりに使います。
/// </summary>
enum class ResourceState : uint8_t
{
    Common,
    VertexAndConstantBuffer,
    IndexBuffer,
    RenderTarget,
    DepthWrite,
    DepthRead,
    ShaderResource,
    UnorderedAccess,
    CopySource,
    CopyDest,
    Present,
    GenericRead,
};

/// <summary>
/// バッファの用途です。組み合わせて使います。
/// </summary>
enum class BufferUsage : uint32_t
{
    None = 0,
    Constant = 1 << 0,
    Structured = 1 << 1,
    ReadWrite = 1 << 2,
    Vertex = 1 << 3,
    Index = 1 << 4,
    Indirect = 1 << 5,
};

constexpr BufferUsage operator|(BufferUsage a, BufferUsage b)
{
    return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

constexpr bool HasFlag(BufferUsage value, BufferUsage flag)
{
    return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

/// <summary>
/// メモリの種類です。
/// </summary>
enum class MemoryType : uint8_t
{
    GpuOnly,
    // CPUから書き込むメモリ。フレーム・イン・フライト分のコピーを内部で持ち、書き込みはそのフレームのコピーに行う
    Upload,
    Readback,
};

struct BufferDesc
{
    uint64_t size = 0;
    uint32_t stride = 0;
    BufferUsage usage = BufferUsage::None;
    MemoryType memory = MemoryType::GpuOnly;
    // GpuOnlyの場合、作成時にこのデータをアップロードする
    const void *initial_data = nullptr;
    std::string debug_name;
};

enum class TextureUsage : uint32_t
{
    None = 0,
    ShaderResource = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
};

constexpr TextureUsage operator|(TextureUsage a, TextureUsage b)
{
    return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

constexpr bool HasFlag(TextureUsage value, TextureUsage flag)
{
    return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

enum class TextureDimension : uint8_t
{
    Texture2D,
    Texture2DArray,
    TextureCube,
};

struct TextureDesc
{
    uint32_t width = 1;
    uint32_t height = 1;
    // Texture2DArrayのスライス数。TextureCubeは6
    uint32_t array_size = 1;
    TextureDimension dimension = TextureDimension::Texture2D;
    uint32_t sample_count = 1;
    Format format = Format::R8G8B8A8_UNorm;
    TextureUsage usage = TextureUsage::ShaderResource;
    std::array<float, 4> clear_color = {0, 0, 0, 1};
    float clear_depth = 1.0f;
    // 作成時にアップロードするピクセルデータ（mip 0、スライス0のみ）
    const void *initial_data = nullptr;
    // 1行（ブロック圧縮の場合は4ピクセル分の1行）のバイト数
    uint32_t initial_row_pitch = 0;
    std::string debug_name;
};

enum class ShaderStage : uint8_t
{
    Vertex,
    Pixel,
};

/// <summary>
/// HLSLのソースです。RHIの中でDXILにコンパイルします。
/// </summary>
struct ShaderSource
{
    std::wstring file_path;
    std::wstring entry_point;
    ShaderStage stage = ShaderStage::Vertex;
    std::vector<std::wstring> defines;
};

struct VertexAttribute
{
    std::string semantic;
    uint32_t semantic_index = 0;
    Format format = Format::Unknown;
    uint32_t offset = 0;

    bool operator==(const VertexAttribute &) const = default;
};

struct VertexLayout
{
    std::vector<VertexAttribute> attributes;
    uint32_t stride = 0;

    bool operator==(const VertexLayout &) const = default;
};

enum class BlendMode : uint8_t
{
    Opaque,
    AlphaBlend,
    Additive,
};

enum class CullMode : uint8_t
{
    None,
    Front,
    Back,
};

enum class CompareOp : uint8_t
{
    Never,
    Less,
    LessEqual,
    Equal,
    Greater,
    GreaterEqual,
    NotEqual,
    Always,
};

enum class PrimitiveTopology : uint8_t
{
    TriangleList,
    LineList,
};

struct DepthState
{
    bool test_enable = true;
    bool write_enable = true;
    CompareOp compare = CompareOp::LessEqual;

    bool operator==(const DepthState &) const = default;
};

/// <summary>
/// ブレンド、カリング、深度の設定です。
/// </summary>
struct RenderState
{
    BlendMode blend = BlendMode::Opaque;
    CullMode cull = CullMode::Back;
    DepthState depth;

    bool operator==(const RenderState &) const = default;
};

struct PipelineStateDesc
{
    ShaderHandle vertex_shader;
    ShaderHandle pixel_shader;
    VertexLayout vertex_layout;
    RenderState render_state;
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    uint32_t render_target_count = 1;
    std::array<Format, kMaxRenderTargets> render_target_formats = {};
    Format depth_format = Format::D32_Float;
    uint32_t sample_count = 1;

    bool operator==(const PipelineStateDesc &) const = default;
    size_t Hash() const;
};

enum class Filter : uint8_t
{
    Point,
    Linear,
    Anisotropic,
};

enum class AddressMode : uint8_t
{
    Wrap,
    Clamp,
    Mirror,
};

struct SamplerDesc
{
    Filter filter = Filter::Linear;
    AddressMode address = AddressMode::Wrap;
    uint32_t max_anisotropy = 8;
};

/// <summary>
/// その場で書き込む頂点データです（線やUIなど、毎フレーム作り直すもの）。SetVertexDataの中でアップロード用のリングにコピーされます。
/// </summary>
struct TransientVertices
{
    const void *data = nullptr;
    uint32_t size = 0;
    uint32_t stride = 0;
};

struct Viewport
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    float min_depth = 0;
    float max_depth = 1;
};

struct ScissorRect
{
    int32_t left = 0;
    int32_t top = 0;
    int32_t right = 0;
    int32_t bottom = 0;
};

enum class LoadOp : uint8_t
{
    Load,
    Clear,
    DontCare,
};

/// <summary>
/// 描画先の設定です。描画先のTransitionは呼び出し側が先に行います。
/// </summary>
struct RenderPassDesc
{
    std::array<TextureHandle, kMaxRenderTargets> color_targets = {};
    uint32_t color_target_count = 0;
    LoadOp color_load = LoadOp::Load;
    std::array<float, 4> clear_color = {0, 0, 0, 1};
    TextureHandle depth_target;
    // 配列テクスチャの場合に描くスライス。kAllSlicesなら全体
    uint32_t depth_slice = kAllSlices;
    LoadOp depth_load = LoadOp::Load;
    float clear_depth = 1.0f;
};

/// <summary>
/// 定数のバインドです。CPUの値（SetBindingsの中でアップロード用のリングにコピーされる）か、GPUバッファのどちらかを渡します。
/// </summary>
struct ConstantBinding
{
    BufferHandle buffer;
    const void *data = nullptr;
    uint32_t size = 0;

    static ConstantBinding FromBuffer(const BufferHandle buffer)
    {
        return {buffer, nullptr, 0};
    }

    static ConstantBinding FromData(const void *data, const uint32_t size)
    {
        return {{}, data, size};
    }

    template <typename T>
    static ConstantBinding FromValue(const T &value)
    {
        return FromData(&value, sizeof(T));
    }
};

/// <summary>
/// t レジスタに見せるリソースです。テクスチャかバッファのどちらかを入れます。
/// </summary>
struct ResourceBinding
{
    TextureHandle texture;
    BufferHandle buffer;
};

/// <summary>
/// 「どのスロットに何を見せるか」です。
/// </summary>
/// <summary>
/// space1 の定数のスロット番号です。
/// </summary>
enum FrameConstantSlot : uint32_t
{
    kFrameConstant_View = 0,
    kFrameConstant_Object = 1,
    kFrameConstant_Lighting = 2,
};

struct Bindings
{
    // b0..b3, space0（マテリアル）
    std::array<ConstantBinding, kConstantSlotCount> constants = {};
    // b0..b2, space1（View, Object, Lighting）
    std::array<ConstantBinding, kFrameConstantSlotCount> frame_constants = {};
    // t0..t7, space0（マテリアル）
    std::array<ResourceBinding, kResourceSlotCount> resources = {};
    // t0..t7, space1（ライト、影、ボーンなど）
    std::array<ResourceBinding, kFrameResourceSlotCount> frame_resources = {};
    std::array<SamplerHandle, kSamplerSlotCount> samplers = {};
};

struct ReadbackTicket
{
    uint64_t id = 0;
};
}
