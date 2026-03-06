#pragma once
#include "generic_material_data.h"
#include "instance_info.h"
#include "object_pool.h"
#include "raytracing_shader.h"
#include "shader_table.h"
#include "top_level_acceleration_structure.h"
#include "Components/camera_component.h"
#include "Rendering/uav_texture.h"
#include "Rendering/view_projection.h"
#include "Rendering/CabotEngine/Graphics/byte_address_buffer.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
struct RaytracingRequest
{
    DescriptorHandle uav_handle;
    std::shared_ptr<CameraComponent> target_camera;
    std::shared_ptr<UavTexture> uav_texture;
    std::shared_ptr<ShaderTable> shader_table;
    std::shared_ptr<RaytracingShader> raytracing_shader;
};

class RaytracingManager
{
    constexpr static size_t kStableCameraCount = 8;
    
    std::vector<std::shared_ptr<MeshRenderer>> m_no_mesh_mesh_renderers_;
    std::vector<std::shared_ptr<MeshRenderer>> m_mesh_renderers_;

    inline const static std::function<std::shared_ptr<ConstantBuffer>()> kOnViewProjBuffCreate = [] {
        auto view_proj_buff = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
        view_proj_buff->CreateBuffer();
        return view_proj_buff;
    };
    std::array<ObjectPool<std::shared_ptr<ConstantBuffer>>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_
        = {ObjectPool(0, kOnViewProjBuffCreate), ObjectPool(0, kOnViewProjBuffCreate)};
    
    std::vector<InstanceInfo> m_instance_infos_;
    std::shared_ptr<StructuredBuffer> m_instance_info_buffer_;

    std::vector<RaytracingRequest> m_requests_;

    std::vector<GenericMaterialData> m_generic_material_datas_;
    std::shared_ptr<StructuredBuffer> m_material_buffer_;

    std::vector<ByteAddressBuffer> m_vertex_address_buffers_;
    std::vector<ByteAddressBuffer> m_index_address_buffers_;
    std::vector<DescriptorHandle> m_vertex_buffer_handle_;
    std::vector<DescriptorHandle> m_index_buffer_handle_;

    std::shared_ptr<TopLevelAccelerationStructure> m_tlas_;
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> m_tlas_instances_;

    void ExecuteRequest(const RaytracingRequest &raytracing_request);
    void Submit(const std::shared_ptr<MeshRenderer> &mesh_renderer);

    void UpdateBuffers();
    void UpdateVertexIndexBuffer();
    void UpdateTransform(int index, const Matrix &matrix);
    void UpdateMaterial(int index, std::vector<AssetPtr<Material>> &materials);

public:
    
    static RaytracingManager *Instance();

    static void Init();
    static void RequestRaytracing(const std::shared_ptr<CameraComponent> &target_camera, const std::shared_ptr<RaytracingShader> &raytracing_shader, const std::shared_ptr<ShaderTable> &shader_table, const std::shared_ptr<UavTexture> &uav_texture);
    
    void Execute();

    void RegisterMeshRenderer(std::shared_ptr<MeshRenderer> mesh_renderer);
    void UnRegisterMeshRenderer(const std::shared_ptr<MeshRenderer> &mesh_renderer);
};
}