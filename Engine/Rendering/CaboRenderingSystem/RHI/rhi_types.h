#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
// 同時に処理中にできるフレーム数。Uploadメモリのバッファやアップロード用のリングは、この数だけコピーを持つ
constexpr uint32_t kMaxFramesInFlight = 2;
// 1回の描画で同時に書き込める色の描画先の数（D3D12の上限と同じ）
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
/// ブロック圧縮（4x4ピクセルを1ブロックとして保存する）のフォーマットかどうかです。
/// 行のバイト数や行数の計算がピクセル単位ではなくブロック単位になります。
/// </summary>
constexpr bool IsBlockCompressed(const Format format)
{
    return format == Format::BC1_UNorm || format == Format::BC2_UNorm || format == Format::BC3_UNorm;
}

/// <summary>
/// リソースの状態です。D3D12_RESOURCE_STATESの代わりに使います。
/// ICommandList::Transitionで、使い方に合わせて切り替えます。
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
    // Uploadメモリのリソースの状態（CPUから書き込み、GPUから読むだけ）
    GenericRead,
};

/// <summary>
/// バッファの用途です。組み合わせて使います。作成するビュー（SRVなど）やリソースのフラグがこれで決まります。
/// </summary>
enum class BufferUsage : uint32_t
{
    None = 0,
    Constant = 1 << 0,
    // StructuredBufferとしてシェーダーから読む（strideが必要）
    Structured = 1 << 1,
    ReadWrite = 1 << 2,
    Vertex = 1 << 3,
    Index = 1 << 4,
    Indirect = 1 << 5,
};

/// <summary>
/// 用途のフラグを組み合わせます。
/// </summary>
constexpr BufferUsage operator|(BufferUsage a, BufferUsage b)
{
    return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

/// <summary>
/// valueにflagが含まれているかどうかです。
/// </summary>
constexpr bool HasFlag(BufferUsage value, BufferUsage flag)
{
    return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

/// <summary>
/// メモリの種類です。
/// </summary>
enum class MemoryType : uint8_t
{
    // GPUだけが読み書きする。CPUからの書き込みはアップロード用のバッファを経由する
    GpuOnly,
    // CPUから書き込むメモリ。フレーム・イン・フライト分のコピーを内部で持ち、書き込みはそのフレームのコピーに行う
    Upload,
    // GPUの結果をCPUで読むためのメモリ
    Readback,
};

/// <summary>
/// バッファを作るときの設定です。
/// </summary>
struct BufferDesc
{
    uint64_t size = 0;
    // 1要素のバイト数。StructuredBufferと頂点バッファで使う
    uint32_t stride = 0;
    BufferUsage usage = BufferUsage::None;
    MemoryType memory = MemoryType::GpuOnly;
    // GpuOnlyの場合、作成時にこのデータをアップロードする
    const void *initial_data = nullptr;
    // PIXなどのデバッガーに表示する名前
    std::string debug_name;
};

/// <summary>
/// テクスチャの用途です。組み合わせて使います。作成するビュー（SRV / RTV / DSV）がこれで決まります。
/// </summary>
enum class TextureUsage : uint32_t
{
    None = 0,
    ShaderResource = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
};

/// <summary>
/// 用途のフラグを組み合わせます。
/// </summary>
constexpr TextureUsage operator|(TextureUsage a, TextureUsage b)
{
    return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

/// <summary>
/// valueにflagが含まれているかどうかです。
/// </summary>
constexpr bool HasFlag(TextureUsage value, TextureUsage flag)
{
    return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

/// <summary>
/// テクスチャの形です。シェーダーから見えるSRVの種類（Texture2D / Texture2DArray / TextureCube）になります。
/// </summary>
enum class TextureDimension : uint8_t
{
    Texture2D,
    Texture2DArray,
    TextureCube,
};

/// <summary>
/// テクスチャを作るときの設定です。
/// </summary>
struct TextureDesc
{
    uint32_t width = 1;
    uint32_t height = 1;
    // Texture2DArrayのスライス数。TextureCubeは6
    uint32_t array_size = 1;
    TextureDimension dimension = TextureDimension::Texture2D;
    // MSAAのサンプル数。1ならMSAAなし
    uint32_t sample_count = 1;
    Format format = Format::R8G8B8A8_UNorm;
    TextureUsage usage = TextureUsage::ShaderResource;
    // RenderTargetのクリアに最適化する色（違う色でクリアすると少し遅くなる）
    std::array<float, 4> clear_color = {0, 0, 0, 1};
    // DepthStencilのクリアに最適化する値
    float clear_depth = 1.0f;
    // 作成時にアップロードするピクセルデータ（mip 0）。配列テクスチャは全スライスを続けて並べる
    const void *initial_data = nullptr;
    // 1行（ブロック圧縮の場合は4ピクセル分の1行）のバイト数
    uint32_t initial_row_pitch = 0;
    // PIXなどのデバッガーに表示する名前
    std::string debug_name;
};

/// <summary>
/// シェーダーの種類です。
/// </summary>
enum class ShaderStage : uint8_t
{
    Vertex,
    Pixel,
};

/// <summary>
/// HLSLのソースです。RHIの中でDXC（シェーダーモデル6）でDXILにコンパイルします。
/// </summary>
struct ShaderSource
{
    std::wstring file_path;
    // エントリーポイントの関数名（VSMain、PSMainなど）
    std::wstring entry_point;
    ShaderStage stage = ShaderStage::Vertex;
    // "NAME" または "NAME=VALUE" の形のマクロ定義
    std::vector<std::wstring> defines;
};

/// <summary>
/// 頂点の1つの要素（位置、法線など）です。シェーダーの入力のセマンティクスと対応させます。
/// </summary>
struct VertexAttribute
{
    // "POSITION"、"NORMAL" など
    std::string semantic;
    // TEXCOORD1 のように同じセマンティクスが複数ある場合の番号
    uint32_t semantic_index = 0;
    Format format = Format::Unknown;
    // 頂点の先頭からのバイト数
    uint32_t offset = 0;

    bool operator==(const VertexAttribute &) const = default;
};

/// <summary>
/// 頂点の並び方です。PSOの入力レイアウトになります。
/// </summary>
struct VertexLayout
{
    std::vector<VertexAttribute> attributes;
    // 1頂点のバイト数
    uint32_t stride = 0;

    bool operator==(const VertexLayout &) const = default;
};

/// <summary>
/// 色の合成方法です。
/// </summary>
enum class BlendMode : uint8_t
{
    Opaque,
    // 通常の半透明（src * a + dst * (1 - a)）
    AlphaBlend,
    // 加算（src * a + dst）
    Additive,
};

/// <summary>
/// 描かない面です。表面は反時計回り（右手系）です。
/// </summary>
enum class CullMode : uint8_t
{
    None,
    Front,
    Back,
};

/// <summary>
/// 深度テストなどの比較方法です。
/// </summary>
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

/// <summary>
/// 頂点のつなぎ方です。
/// </summary>
enum class PrimitiveTopology : uint8_t
{
    TriangleList,
    LineList,
};

/// <summary>
/// 深度テストと深度の書き込みの設定です。
/// </summary>
struct DepthState
{
    bool test_enable = true;
    bool write_enable = true;
    CompareOp compare = CompareOp::LessEqual;

    bool operator==(const DepthState &) const = default;
};

/// <summary>
/// ブレンド、カリング、深度の設定です。Materialのパスごとに持ち、PSOを引くときに使います。
/// </summary>
struct RenderState
{
    BlendMode blend = BlendMode::Opaque;
    CullMode cull = CullMode::Back;
    DepthState depth;

    bool operator==(const RenderState &) const = default;
};

/// <summary>
/// PSOを作るときの設定です。IRenderDevice::CreatePipelineStateは同じ設定のPSOをキャッシュから返します。
/// </summary>
struct PipelineStateDesc
{
    ShaderHandle vertex_shader;
    // 無効なハンドルなら、ピクセルシェーダーなし（影などの深度だけの描画）
    ShaderHandle pixel_shader;
    VertexLayout vertex_layout;
    RenderState render_state;
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    uint32_t render_target_count = 1;
    std::array<Format, kMaxRenderTargets> render_target_formats = {};
    // Unknownなら深度バッファなし
    Format depth_format = Format::D32_Float;
    uint32_t sample_count = 1;

    bool operator==(const PipelineStateDesc &) const = default;
    /// <summary>
    /// PSOのキャッシュのキーにするハッシュ値です。全メンバーを順番も区別して混ぜます。
    /// </summary>
    size_t Hash() const;
};

/// <summary>
/// テクスチャを読むときの補間方法です。
/// </summary>
enum class Filter : uint8_t
{
    Point,
    Linear,
    Anisotropic,
};

/// <summary>
/// UVが0..1の外に出たときの扱いです。
/// </summary>
enum class AddressMode : uint8_t
{
    Wrap,
    Clamp,
    Mirror,
};

/// <summary>
/// サンプラーを作るときの設定です。
/// </summary>
struct SamplerDesc
{
    Filter filter = Filter::Linear;
    AddressMode address = AddressMode::Wrap;
    // Anisotropicのときだけ使う
    uint32_t max_anisotropy = 8;
};

/// <summary>
/// その場で書き込む頂点データです（線やUIなど、毎フレーム作り直すもの）。SetVertexDataの中でアップロード用のリングにコピーされます。
/// </summary>
struct TransientVertices
{
    const void *data = nullptr;
    // 全体のバイト数
    uint32_t size = 0;
    // 1頂点のバイト数
    uint32_t stride = 0;
};

/// <summary>
/// 描画する範囲（ピクセル）と深度の範囲です。
/// </summary>
struct Viewport
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    float min_depth = 0;
    float max_depth = 1;
};

/// <summary>
/// 描画を切り抜く範囲（ピクセル）です。rightとbottomは含みません。
/// </summary>
struct ScissorRect
{
    int32_t left = 0;
    int32_t top = 0;
    int32_t right = 0;
    int32_t bottom = 0;
};

/// <summary>
/// 描画を始めるときに、描画先の今の内容をどうするかです。
/// </summary>
enum class LoadOp : uint8_t
{
    // 残す
    Load,
    // クリアする
    Clear,
    // どうなってもよい（全面を描き直す場合）
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
    // 無効なハンドルなら深度バッファなし
    TextureHandle depth_target;
    // 配列テクスチャの場合に描くスライス。kAllSlicesなら全体
    uint32_t depth_slice = kAllSlices;
    LoadOp depth_load = LoadOp::Load;
    float clear_depth = 1.0f;
};

/// <summary>
/// 定数のバインドです。CPUの値（SetBindingsの中でアップロード用のリングにコピーされる）か、GPUバッファのどちらかを渡します。
/// 何も設定しない場合は、0で埋めた領域が渡されます。
/// </summary>
struct ConstantBinding
{
    BufferHandle buffer;
    const void *data = nullptr;
    uint32_t size = 0;

    /// <summary>
    /// GPUバッファをそのまま使います。
    /// </summary>
    static ConstantBinding FromBuffer(const BufferHandle buffer)
    {
        return {buffer, nullptr, 0};
    }

    /// <summary>
    /// CPUのバイト列を使います。SetBindingsを呼ぶまでdataを生かしておいてください。
    /// </summary>
    static ConstantBinding FromData(const void *data, const uint32_t size)
    {
        return {{}, data, size};
    }

    /// <summary>
    /// CPUの値を使います。SetBindingsを呼ぶまでvalueを生かしておいてください。
    /// </summary>
    template <typename T>
    static ConstantBinding FromValue(const T &value)
    {
        return FromData(&value, sizeof(T));
    }
};

/// <summary>
/// t レジスタに見せるリソースです。テクスチャかバッファのどちらかを入れます。両方空なら空のSRVになります。
/// </summary>
struct ResourceBinding
{
    TextureHandle texture;
    BufferHandle buffer;
};

/// <summary>
/// space1 の定数のスロット番号です。
/// </summary>
enum FrameConstantSlot : uint32_t
{
    kFrameConstant_View = 0,
    kFrameConstant_Object = 1,
    kFrameConstant_Lighting = 2,
};

/// <summary>
/// 「どのスロットに何を見せるか」です。共通ルートシグネチャと同じ形をしていて、ICommandList::SetBindingsで一度に設定します。
/// </summary>
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
    // s0..s3, space0（マテリアル）。無効なハンドルは既定のサンプラーになる
    std::array<SamplerHandle, kSamplerSlotCount> samplers = {};
};

/// <summary>
/// RequestReadbackの受付番号です。TryGetReadbackに渡して、GPUの処理が終わった後に結果を受け取ります。
/// </summary>
struct ReadbackTicket
{
    uint64_t id = 0;
};
}
