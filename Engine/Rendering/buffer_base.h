#pragma once

namespace engine
{
enum kGpuUploadType
{
    kParameterBufferType_CBV,
    kParameterBufferType_SRV,
    kParameterBufferType_UAV,

    kParameterBufferType_Count
};

class DescriptorHandle;

class BufferBase : public Object
{
public:
    bool is_dirty;

    ~BufferBase() override = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer(void *data) = 0;
    virtual void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle) = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual kGpuUploadType BufferType() const = 0;
    virtual bool IsValid() = 0;
};
}