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

class GpuResourceGroup
{
    ShaderDataIndex m_shader_index_;
    bool m_is_dirty_ = true;
    using BufferIsExternalPair = std::pair<AssetPtr<BufferBase>, bool>;
    using BufferHandlePair = std::pair<BufferIsExternalPair, std::shared_ptr<DescriptorHandle>>;
    std::vector<BufferHandlePair> m_buffers_;

public:

    void Insert(const AssetPtr<BufferBase> &buffer, bool is_external = false);
    bool Empty(kGpuUploadType buffer_type);
    std::vector<BufferHandlePair>::iterator Begin(kGpuUploadType buffer_type);
    std::vector<BufferHandlePair>::iterator End(kGpuUploadType buffer_type);

    
    void UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block);
    void SetBufferToDescriptorTable();
};
}