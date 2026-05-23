#pragma once
#include "buffer_base.h"
#include "material_block.h"
#include "Asset/asset_ptr.h"

namespace engine
{
struct ShaderDataIndex
{
    int cbv_length = 0;
    int srv_length = 0;
    int uav_length = 0;

    int *GetLengthField(kGpuUploadType type);
    int GetLength(kGpuUploadType type) const;
    int GetOffset(kGpuUploadType type) const;
    int GetFullLength() const;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(cbv_length), CEREAL_NVP(srv_length), CEREAL_NVP(uav_length));
    }
};

struct GpuResource
{
    bool is_external;
    std::string name;
    std::shared_ptr<BufferBase> buffer;
    std::shared_ptr<DescriptorHandle> handle;
    kBufferType buffer_type;
};

class GpuResourceGroup
{
    ShaderDataIndex m_shader_index_;
    bool m_is_dirty_ = true;
    std::vector<GpuResource> m_gpu_resources_;

    static void UpdateConstantBuffer(const GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    static void UpdateStructuredBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    static void UpdateTextureBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    
public:
    void Insert(const std::shared_ptr<BufferBase> &buffer, const std::shared_ptr<MaterialDataBase> &material_data, kBufferType buffer_type, bool is_external = false);
    bool Empty(kGpuUploadType buffer_type) const;
    std::vector<GpuResource>::iterator Begin(kGpuUploadType buffer_type);
    std::vector<GpuResource>::iterator End(kGpuUploadType buffer_type);

    std::shared_ptr<BufferBase> FindBufferWithName(const std::string &name);
    bool SetBufferWithName(const std::shared_ptr<BufferBase> &buffer, const std::string &name);

    bool UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block) const;
    bool SetBufferToDescriptorTable();
    
};
}