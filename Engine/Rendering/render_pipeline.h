#pragma once
#include "camera.h"
#include "event.h"
#include "material.h"
#include "object_pool.h"
#include "render_command.h"
#include "uav_texture.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/sub_descriptorheap.h"
#include "raytracing/bottom_level_acceleration_structure.h"
#include "raytracing/raytrace_pass.h"
#include "raytracing/raytracing_shader.h"
#include "raytracing/shader_table.h"
#include "raytracing/top_level_acceleration_structure.h"

namespace engine
{
struct CameraProperty;
class Renderer;
class DepthTexture;

class RenderPipeline
{
    friend class Light;
    friend class Engine;
    friend class CameraComponent;

    constexpr static uint32_t kStaticDescriptorHeapCount = 50000;

    constexpr static size_t kStableCameraCount = 8;
    constexpr static size_t kReserveRendererCount = 1000;
    inline const static std::function<std::shared_ptr<ConstantBuffer>()> kOnViewProjBuffCreate = [] {
        auto view_proj_buff = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
        view_proj_buff->CreateBuffer();
        return view_proj_buff;
    };

    std::shared_ptr<SubDescriptorHeap> m_static_descriptor_heap_;
    std::array<std::shared_ptr<SubDescriptorHeap>, RenderEngine::kFrame_Buffer_Count> m_dynamic_descriptor_heaps_;
    std::vector<RenderCommand> m_commands_;
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> m_tlas_instances_;
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::shared_ptr<ConstantBuffer> m_scene_data_buffer_;

    Camera m_current_camera_;
    std::vector<Camera> m_requesting_cameras_;
    uint32_t m_current_view_proj_matrix_index_;
    std::array<ObjectPool<std::shared_ptr<ConstantBuffer>>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_
        = {ObjectPool(0, kOnViewProjBuffCreate), ObjectPool(0, kOnViewProjBuffCreate)};

    std::vector<std::shared_ptr<RaytracePass>> m_raytrace_passes_;
    std::vector<GenericMaterialData> m_generic_material_datas_;
    std::shared_ptr<StructuredBuffer> m_material_buffer_;
    std::shared_ptr<UavTexture> m_uav_texture_;
    std::shared_ptr<ShaderTable> m_shader_table_;
    std::shared_ptr<RaytracingShader> m_raytracing_shader_;
    std::shared_ptr<TopLevelAccelerationStructure> m_tlas_;
    DescriptorHandle m_uav_texture_handle_;

    void InvokeDrawCall();

    void RenderMainRenderTarget(const std::shared_ptr<CameraComponent> &main_camera);
    void RenderCamera(const Camera &camera);
    void RenderVoid();
    void Render(const Matrix &view, const Matrix &proj);
    
    void SetCurrentCamera(const Camera &camera);
    void SetSceneData();
    void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    void UpdateBuffer(const Matrix &view, const Matrix &proj);
    void DepthRender() const;
    void ExecuteRenderCommands();
    void RayTracingRender();

public:
    Event<> on_rendering;
    
    static void Init();
    static RenderPipeline *Instance();
    static size_t GetRendererCount();
    static Camera GetCurrentCamera();
    static std::shared_ptr<SubDescriptorHeap> GetStaticDescriptorHeap();
    static std::shared_ptr<SubDescriptorHeap> GetDynamicDescriptorHeap();
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth, const Shader &shader);

    static void SubmitRaytracing(const std::shared_ptr<Mesh> &mesh, std::vector<AssetPtr<Material>> &materials, const Matrix &matrix);
    static void Submit(const std::shared_ptr<Mesh> &mesh, std::vector<AssetPtr<Material>> &materials, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address = {}, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle = {});
    static void Submit(AssetPtr<FontData> font_data, Vector2 position, const std::string &string, Color color);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
    static void RequestRender(Camera camera);
    static void AddRaytracePass(std::shared_ptr<RaytracePass> raytrace_pass);
};
}