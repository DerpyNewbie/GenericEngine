#pragma once
class DescriptorHandle;

namespace engine
{
enum kParameterBufferType
{
    kParameterBufferType_ConstantBuffer,
    kParameterBufferType_StructuredBuffer,
    kParameterBufferType_Texture2D,

    kParameterBufferType_Count
};
}

class IBuffer : public engine::Object, public engine::Inspectable
{
public:
    virtual ~IBuffer() = default;
    virtual bool CreateBuffer() = 0;
    virtual void UpdateBuffer() = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual engine::kParameterBufferType BufferType() const = 0;
    virtual bool IsValid() = 0;
};