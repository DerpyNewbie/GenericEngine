#pragma once
class DescriptorHandle;

class IBuffer
{
public:
    virtual ~IBuffer() = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer(void *data) = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual bool CanUpdate() = 0;
    virtual bool IsValid() = 0;
};