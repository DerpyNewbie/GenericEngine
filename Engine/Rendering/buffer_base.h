#pragma once

namespace engine
{
enum kGpuUploadType : int
{
    kGpuBufferType_CBV = 0,
    kGpuBufferType_SRV = 1,
    kGpuBufferType_UAV = 2,

    kGpuBufferType_Count = 3
    
};

class DescriptorHandle;

class BufferBase
{
public:
    bool is_dirty;

    virtual ~BufferBase() = default;
    virtual void CreateBuffer() = 0;
    virtual void UpdateBuffer(const void *data) = 0;
    virtual void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) = 0;
    virtual std::shared_ptr<DescriptorHandle> UploadBuffer() = 0;
    virtual bool IsValid() = 0;
    virtual bool Transition(D3D12_RESOURCE_STATES new_state) = 0;
};
}