#pragma once
#include <cstdint>

namespace CRS
{
/// <summary>
/// GPUリソースを指すハンドルです。世代番号で破棄済みのハンドルの使用を検出します。Tagで種類ごとに別の型にします。
/// </summary>
template <typename Tag>
struct Handle
{
    static constexpr uint32_t kInvalidIndex = UINT32_MAX;

    uint32_t index = kInvalidIndex;
    uint32_t generation = 0;

    /// <summary>
    /// 無効値（何も指していない）でないかどうかです。指す先がまだ生きているかは、SlotPool::Getなどで確かめてください。
    /// </summary>
    bool IsValid() const
    {
        return index != kInvalidIndex;
    }

    bool operator==(const Handle &) const = default;
};

using BufferHandle = Handle<struct BufferTag>;
using TextureHandle = Handle<struct TextureTag>;
using ShaderHandle = Handle<struct ShaderTag>;
using PipelineStateHandle = Handle<struct PipelineStateTag>;
using SamplerHandle = Handle<struct SamplerTag>;
}
