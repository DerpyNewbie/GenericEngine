#pragma once
#include "generic_material_data.h"
#include "instance_info.h"
#include "raytracing_shader.h"
#include "shader_table.h"
#include "top_level_acceleration_structure.h"
#include "Rendering/uav_texture.h"
#include "Rendering/CabotEngine/Graphics/byte_address_buffer.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
class RaytracingManager
{
    std::vector<std::shared_ptr<MeshRenderer>> m_no_mesh_mesh_renderers_;
    std::vector<std::shared_ptr<MeshRenderer>> m_mesh_renderers_;

    DescriptorHandle m_uav_texture_handle_; //TODO : こいつの所在を考えるべき
    std::vector<std::shared_ptr<UavTexture>> m_uav_textures_;
    std::vector<std::shared_ptr<RaytracingShader>> m_raytracing_shaders_;

    std::shared_ptr<ShaderTable> m_shader_table_; //TODO : こいつをRaytracingShaderの中に入れるべきかも

    std::vector<InstanceInfo> m_instance_infos_;
    std::shared_ptr<StructuredBuffer> m_instance_info_buffer_;

    std::vector<GenericMaterialData> m_generic_material_datas_; //TODO : あわわ
    std::shared_ptr<StructuredBuffer> m_material_buffer_;

    std::vector<ByteAddressBuffer> m_vertex_address_buffers_;
    std::vector<ByteAddressBuffer> m_index_address_buffers_;
    std::vector<DescriptorHandle> m_vertex_buffer_handle_;
    std::vector<DescriptorHandle> m_index_buffer_handle_;

    std::shared_ptr<TopLevelAccelerationStructure> m_tlas_;
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> m_tlas_instances_;

    std::shared_ptr<ConstantBuffer> m_view_proj_buffers_[RenderEngine::kFrame_Buffer_Count]; //TODO : カメラ指定できるようにしたほうがいいかも

    void Submit(const std::shared_ptr<MeshRenderer> &mesh_renderer);

    void UpdateBuffers();
    void UpdateVertexIndexBuffer();
    void UpdateTransform(int index, const Matrix &matrix);
    void UpdateMaterial(int index, std::vector<AssetPtr<Material>> &materials);

public:
    static RaytracingManager *Instance();

    static void Init();
    void Execute();

    void RegisterMeshRenderer(std::shared_ptr<MeshRenderer> mesh_renderer);
    void UnRegisterMeshRenderer(const std::shared_ptr<MeshRenderer> &mesh_renderer);
};
}