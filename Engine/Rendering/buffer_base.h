#pragma once
class DescriptorHandle;

namespace engine
{
enum kGpuUploadType
{
    kParameterBufferType_CBV,
    kParameterBufferType_SRV,
    kParameterBufferType_UAV,

    kParameterBufferType_Count
};
}

class BufferBase : public engine::Object
{
public:
    bool is_dirty;

    ~BufferBase() override = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer(void *data) = 0;
    virtual void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle) = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual engine::kGpuUploadType BufferType() const = 0;
    virtual bool IsValid() = 0;
};