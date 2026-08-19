#pragma once
#include "camera.h"
#include "compute_command.h"
#include "compute_shader.h"
#include "event.h"
#include "material.h"
#include "object_pool.h"
#include "render_command.h"
#include "render_texture.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"

namespace engine
{
class GpuResourceGroup;
struct CameraProperty;
class Renderer;
class DepthTexture;

class RenderPipeline
{
    friend class Light;
    friend class Engine;
    friend class CameraComponent;

    constexpr static size_t kStableCameraCount = 8;
    inline const static std::function<std::shared_ptr<ConstantBuffer>()> kOnViewProjBuffCreate = [] {
        auto view_proj_buff = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
        view_proj_buff->CreateBuffer();
        return view_proj_buff;
    };
    
    uint16_t m_effect_render_queue_;

    std::shared_ptr<Shader> m_depth_shader_;

    std::vector<RenderCommand> m_render_commands_;
    std::vector<ComputeCommand> m_compute_commands_;
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::shared_ptr<ConstantBufferData> m_scene_data_buffer_data_;

    Camera m_current_camera_;
    std::vector<Camera> m_requesting_cameras_;
    uint32_t m_current_view_proj_matrix_index_;
    ObjectPool<std::shared_ptr<ConstantBuffer>> m_view_proj_matrix_buffers_ = ObjectPool(0, kOnViewProjBuffCreate);

    std::unordered_map<std::shared_ptr<MaterialBlock>, std::shared_ptr<GpuResourceGroup>> m_material_block_gpu_resource_groups_map_;
    void InvokeDrawCall();

    void RenderMainRenderTarget(const std::shared_ptr<CameraComponent> &main_camera);
    void RenderCamera(const Camera &camera);
    void RenderVoid();
    void Render(const Matrix &view, const Matrix &proj);

    void SetEffectCommand();
    void SetCurrentCamera(const Camera &camera);
    void SetSceneData();
    void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    void UpdateBuffer(const Matrix &view, const Matrix &proj);
    void DepthRender();
    
    void ExecuteRenderCommands();
    void ExecuteComputeCommands();

public:
    Event<> on_cmd_list_open;
    Event<> on_rendering;

    static void Init();
    static RenderPipeline *Instance();
    static size_t GetRendererCount();
    static Camera GetCurrentCamera();
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth);
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth, const Shader &shader);

    static void Submit(const AssetPtr<ComputeShader> &compute_shader, const std::shared_ptr<MaterialBlock> &material_block, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);
    static void Submit(const std::shared_ptr<Mesh> &mesh, const std::vector<AssetPtr<Material>> &materials, uint32_t instance_count, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address = {}, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle = {});
    static void Submit(const AssetPtr<FontData> &font_data, Vector2 position, const std::string &string, Color color, float rotation, Vector2 origin, float scale, uint16_t render_queue);
    static void Submit(const std::vector<AssetPtr<Material>> &materials, uint32_t vertex_count);
    static void SetEffectRenderQueue(uint16_t render_queue);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
    static void RequestRender(Camera camera);
};
}