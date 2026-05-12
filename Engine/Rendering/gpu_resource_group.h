#pragma once
#include "BufferBase.h"
#include "material_block.h"
#include "Asset/asset_ptr.h"

namespace engine
{
struct ShaderDataIndex
{
    int cbv_length = 0;
    int srv_length = 0;
    int uav_length = 0;

    int *GetLengthField(kParameterBufferType type);
    int GetLength(kParameterBufferType type) const;
    int GetOffset(kParameterBufferType type) const;
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

public:
    using BufferIsExternalPair = std::pair<AssetPtr<BufferBase>, bool>;
    using BufferHandlePair = std::pair<BufferIsExternalPair, std::shared_ptr<DescriptorHandle>>;
    std::vector<BufferHandlePair> buffers;

    void Insert(const AssetPtr<BufferBase> &buffer, bool is_external = false);
    bool Empty(kParameterBufferType buffer_type);
    std::vector<BufferHandlePair>::iterator Begin(kParameterBufferType buffer_type);
    std::vector<BufferHandlePair>::iterator End(kParameterBufferType buffer_type);

    void UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block);
};
}