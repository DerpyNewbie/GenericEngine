#pragma once
class DescriptorHandle;

namespace engine
{
enum kParameterBufferType
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
    bool is_dirty_;

    ~BufferBase() override = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer(void *data) = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual engine::kParameterBufferType BufferType() const = 0;
    virtual bool IsValid() = 0;
};