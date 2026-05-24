#pragma once
#include "buffer_base.h"
#include "material_block.h"
#include "Asset/asset_ptr.h"

namespace engine
{
struct GpuResource
{
    std::string name;
    std::shared_ptr<BufferBase> buffer;
    std::shared_ptr<DescriptorHandle> handle;
    kBufferType buffer_type;
};

class GpuResourceGroup
{
    bool m_is_dirty_ = true;
    std::array<std::map<int, GpuResource>, kGpuBufferType_Count> m_gpu_resources_;
    
    static void UpdateConstantBuffer(const GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    static void UpdateStructuredBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    static void UpdateTextureBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);

    static bool SetGlobalResource(GpuResource &gpu_resource);
    
public:
    void Insert(const std::shared_ptr<BufferBase> &buffer, const std::shared_ptr<MaterialDataBase> &material_data, kBufferType buffer_type, bool is_external = false);
    bool Empty(kGpuUploadType buffer_type) const;
    GpuResource Begin(kGpuUploadType buffer_type);
    GpuResource End(kGpuUploadType buffer_type);

    bool UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block) const;
    bool SetBufferToDescriptorTable();
};
}