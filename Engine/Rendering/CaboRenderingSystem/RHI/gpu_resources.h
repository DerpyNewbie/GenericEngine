#pragma once
#include <span>
#include <utility>

#include "Rendering/CaboRenderingSystem/RHI/i_render_device.h"

namespace CRS
{
/// <summary>
/// ハンドルとデバイスを持ち、破棄を自動で行うラッパーの基底です。コピーは禁止、ムーブは可能です。
/// </summary>
/// <remarks>デバイスはすべてのラッパーより後に破棄してください。</remarks>
template <typename HandleT>
class GpuResource
{
protected:
    IRenderDevice *m_device_ = nullptr;
    HandleT m_handle_;

    GpuResource() = default;

    GpuResource(IRenderDevice &device, const HandleT handle) : m_device_(&device), m_handle_(handle)
    {}

public:
    ~GpuResource()
    {
        Reset();
    }

    GpuResource(const GpuResource &) = delete;
    GpuResource &operator=(const GpuResource &) = delete;

    GpuResource(GpuResource &&other) noexcept :
        m_device_(std::exchange(other.m_device_, nullptr)), m_handle_(std::exchange(other.m_handle_, HandleT{}))
    {}

    GpuResource &operator=(GpuResource &&other) noexcept
    {
        if (this != &other)
        {
            Reset();
            m_device_ = std::exchange(other.m_device_, nullptr);
            m_handle_ = std::exchange(other.m_handle_, HandleT{});
        }
        return *this;
    }

    /// <summary>
    /// 保持しているリソースを破棄します（遅延破棄）。
    /// </summary>
    void Reset()
    {
        if (m_device_ != nullptr && m_handle_.IsValid())
            m_device_->Destroy(m_handle_);

        m_handle_ = {};
    }

    /// <summary>
    /// ハンドルを取得します。Bindingsなどに渡すときに使います。所有権はこのラッパーに残ります。
    /// </summary>
    HandleT GetHandle() const
    {
        return m_handle_;
    }

    /// <summary>
    /// リソースを持っているかどうかです（作成に失敗した場合やムーブ後はfalse）。
    /// </summary>
    bool IsValid() const
    {
        return m_handle_.IsValid();
    }
};

/// <summary>
/// 汎用のバッファです。型を持たないので、サイズと用途はBufferDescで指定します。
/// </summary>
class Buffer : public GpuResource<BufferHandle>
{
public:
    Buffer() = default;

    /// <summary>
    /// descの設定でバッファを作成します。失敗した場合はIsValid()がfalseになります。
    /// </summary>
    Buffer(IRenderDevice &device, const BufferDesc &desc) : GpuResource(device, device.CreateBuffer(desc))
    {}

    /// <summary>
    /// 内容を書き換えます。Uploadメモリのバッファだけに使えます（GpuOnlyはエラー）。
    /// 今のフレームのコピーにすぐ書き、他のフレームのコピーにはそのフレームのGPU処理が終わってから反映します。
    /// </summary>
    void Update(const void *data, const uint64_t size, const uint64_t offset = 0)
    {
        if (m_device_ != nullptr)
            m_device_->UpdateBuffer(m_handle_, data, size, offset);
    }
};

/// <summary>
/// 型付きの定数バッファです。Uploadメモリに置かれ、Setはそのフレームのコピーに書き込みます。
/// </summary>
template <typename T>
class ConstantBuffer : public Buffer
{
public:
    ConstantBuffer() = default;

    /// <summary>
    /// sizeof(T)の定数バッファを作成します。
    /// </summary>
    explicit ConstantBuffer(IRenderDevice &device, std::string debug_name = "ConstantBuffer") :
        Buffer(device, {sizeof(T), 0, BufferUsage::Constant, MemoryType::Upload, nullptr, std::move(debug_name)})
    {}

    /// <summary>
    /// 値を書き込みます。GPUが前のフレームで読んでいるコピーは書き換えません。
    /// </summary>
    void Set(const T &value)
    {
        Update(&value, sizeof(T));
    }
};

/// <summary>
/// 型付きのStructuredBufferです。シェーダーの StructuredBuffer<T> に対応します（ライトやボーン行列など）。
/// </summary>
template <typename T>
class StructuredBuffer : public Buffer
{
    uint32_t m_count_ = 0;

public:
    StructuredBuffer() = default;

    /// <summary>
    /// count個の要素を持つバッファを作成します。大きさは後から変えられないので、足りなくなったら作り直してください。
    /// </summary>
    /// <param name="extra_usage">Structuredに加える用途（ReadWriteなど）</param>
    StructuredBuffer(IRenderDevice &device, const uint32_t count, const MemoryType memory = MemoryType::Upload,
                     const BufferUsage extra_usage = BufferUsage::None, std::string debug_name = "StructuredBuffer") :
        Buffer(device, {sizeof(T) * count, sizeof(T), BufferUsage::Structured | extra_usage, memory, nullptr,
                        std::move(debug_name)}),
        m_count_(count)
    {}

    /// <summary>
    /// 先頭から要素を書き込みます。valuesはCount()以下にしてください。
    /// </summary>
    void Set(std::span<const T> values)
    {
        Update(values.data(), values.size_bytes());
    }

    /// <summary>
    /// 作成時に指定した要素数です。
    /// </summary>
    uint32_t Count() const
    {
        return m_count_;
    }
};

/// <summary>
/// テクスチャ（2D、配列、キューブ、描画先、深度バッファ）です。
/// </summary>
class Texture : public GpuResource<TextureHandle>
{
public:
    Texture() = default;

    /// <summary>
    /// descの設定でテクスチャを作成します。initial_dataがあれば、作成時にアップロードします（完了まで待ちます）。
    /// </summary>
    Texture(IRenderDevice &device, const TextureDesc &desc) : GpuResource(device, device.CreateTexture(desc))
    {}
};

/// <summary>
/// コンパイル済みのシェーダー（1つのエントリーポイント）と、そのリフレクションです。
/// </summary>
class Shader : public GpuResource<ShaderHandle>
{
public:
    Shader() = default;

    /// <summary>
    /// HLSLをDXCでコンパイルします。失敗した場合はエラーをログに出し、IsValid()がfalseになります。
    /// </summary>
    Shader(IRenderDevice &device, const ShaderSource &source) : GpuResource(device, device.CreateShader(source))
    {}

    /// <summary>
    /// cbufferの変数とリソースのレジスタの一覧です。Materialのパラメーター表を作るのに使います。
    /// </summary>
    const ShaderReflection &Reflection() const
    {
        return m_device_->GetReflection(m_handle_);
    }
};

/// <summary>
/// サンプラー（テクスチャの補間方法とUVの扱い）です。
/// </summary>
class Sampler : public GpuResource<SamplerHandle>
{
public:
    Sampler() = default;

    /// <summary>
    /// descの設定でサンプラーを作成します。
    /// </summary>
    Sampler(IRenderDevice &device, const SamplerDesc &desc) : GpuResource(device, device.CreateSampler(desc))
    {}
};
}
