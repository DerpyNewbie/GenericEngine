#pragma once
#include "gpu_resource_group.h"
#include "material.h"
#include "CabotEngine/Graphics/ByteAddressBuffer.h"

namespace engine
{
class GpuResourceManager
{
    inline static std::map<std::shared_ptr<MaterialBlock>, std::shared_ptr<GpuResourceGroup>> m_material_block_buffer_map_;
    inline static std::unordered_map<std::string, std::shared_ptr<BufferBase>> m_global_resources_;

public:
    static std::shared_ptr<GpuResourceGroup> GetBuffersForMaterial(std::shared_ptr<MaterialBlock> material_block);
    static std::shared_ptr<BufferBase> GetGlobalBuffer(const std::string &name);

    static void SetGlobalBufferData(const std::string &name, const std::shared_ptr<BufferDataBase> &buffer_data);
    static void SetGlobalBuffer(const std::string &name, const std::shared_ptr<ConstantBuffer> &buffer);
    static void SetGlobalBuffer(const std::string &name, const std::shared_ptr<StructuredBuffer> &buffer);
    static void SetGlobalBuffer(const std::string &name, const std::shared_ptr<ByteAddressBuffer> &buffer);
    static void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture2D> &texture);
    static void SetGlobalInt(const std::string &name, int data);
    static void SetGlobalFloat(const std::string &name, float data);
    static void SetGlobalFloatArray(const std::string &name, const std::vector<float> &data);
    static void SetGlobalVector(const std::string &name, const Vector3 &data);
    static void SetGlobalVectorArray(const std::string &name, const std::vector<Vector3> &data);
    static void SetGlobalMatrix(const std::string &name, const Matrix &data);
    static void SetGlobalMatrixArray(const std::string &name, const std::vector<Matrix> &data);
    static void SetGlobalColor(const std::string &name, const Color &data);
};
}