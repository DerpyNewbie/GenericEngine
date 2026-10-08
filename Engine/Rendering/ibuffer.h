#pragma once
class DescriptorHandle;

namespace engine
{
/// <summary>
/// Shaderに渡すBufferの種類です。
/// </summary>
enum kParameterBufferType
{
    kParameterBufferType_CBV,
    kParameterBufferType_SRV,
    kParameterBufferType_UAV,

    kParameterBufferType_Count
};
}

/// <summary>
/// GPUに送るBufferのInterfaceです。
/// </summary>
class IBuffer
{
public:
    virtual ~IBuffer() = default;
    /// <summary>
    /// GPU上のBufferを作成します。
    /// </summary>
    virtual void CreateBuffer() = 0;
    /// <summary>
    /// データをBufferに書き込みます。
    /// </summary>
    /// <param name="data">書き込むデータ</param>
    virtual void UpdateBuffer(void *data) = 0;
    /// <summary>
    /// Bufferを参照するViewをDescriptorHeapに登録します。
    /// </summary>
    /// <returns>登録されたDescriptorHandle</returns>
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    /// <summary>
    /// UpdateBufferで内容を書き換えられるかどうかを取得します。
    /// </summary>
    virtual bool CanUpdate() = 0;
    /// <summary>
    /// Bufferが作成済みであるかどうかを取得します。
    /// </summary>
    virtual bool IsValid() = 0;
};