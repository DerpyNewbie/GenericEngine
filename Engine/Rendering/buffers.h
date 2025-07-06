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
    
    bool IsValid()
    {
        return m_IsValid;
    }

protected:
    bool m_IsValid = false;
};