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

    HandleT GetHandle() const
    {
        return m_handle_;
    }

    bool IsValid() const
    {
        return m_handle_.IsValid();
    }
};

/// <summary>
/// 汎用のバッファです。
/// </summary>
class Buffer : public GpuResource<BufferHandle>
{
public:
    Buffer() = default;

    Buffer(IRenderDevice &device, const BufferDesc &desc) : GpuResource(device, device.CreateBuffer(desc))
    {}

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

    explicit ConstantBuffer(IRenderDevice &device, std::string debug_name = "ConstantBuffer") :
        Buffer(device, {sizeof(T), 0, BufferUsage::Constant, MemoryType::Upload, nullptr, std::move(debug_name)})
    {}

    void Set(const T &value)
    {
        Update(&value, sizeof(T));
    }
};

/// <summary>
/// 型付きのStructuredBufferです。
/// </summary>
template <typename T>
class StructuredBuffer : public Buffer
{
    uint32_t m_count_ = 0;

public:
    StructuredBuffer() = default;

    StructuredBuffer(IRenderDevice &device, const uint32_t count, const MemoryType memory = MemoryType::Upload,
                     const BufferUsage extra_usage = BufferUsage::None, std::string debug_name = "StructuredBuffer") :
        Buffer(device, {sizeof(T) * count, sizeof(T), BufferUsage::Structured | extra_usage, memory, nullptr,
                        std::move(debug_name)}),
        m_count_(count)
    {}

    void Set(std::span<const T> values)
    {
        Update(values.data(), values.size_bytes());
    }

    uint32_t Count() const
    {
        return m_count_;
    }
};

class Texture : public GpuResource<TextureHandle>
{
public:
    Texture() = default;

    Texture(IRenderDevice &device, const TextureDesc &desc) : GpuResource(device, device.CreateTexture(desc))
    {}
};

class Shader : public GpuResource<ShaderHandle>
{
public:
    Shader() = default;

    Shader(IRenderDevice &device, const ShaderSource &source) : GpuResource(device, device.CreateShader(source))
    {}

    const ShaderReflection &Reflection() const
    {
        return m_device_->GetReflection(m_handle_);
    }
};

class Sampler : public GpuResource<SamplerHandle>
{
public:
    Sampler() = default;

    Sampler(IRenderDevice &device, const SamplerDesc &desc) : GpuResource(device, device.CreateSampler(desc))
    {}
};
}
