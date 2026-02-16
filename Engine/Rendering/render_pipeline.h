#pragma once
#include "camera.h"
#include "event.h"
#include "material.h"
#include "object_pool.h"
#include "render_command.h"
#include "render_texture.h"
#include "CabotEngine/Graphics/RenderEngine.h"

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

    std::vector<RenderCommand> m_commands_;
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::shared_ptr<ConstantBuffer> m_scene_data_buffer_;

    Camera m_current_camera_;
    std::vector<Camera> m_requesting_cameras_;
    uint32_t m_current_view_proj_matrix_index_;
    std::array<ObjectPool<std::shared_ptr<ConstantBuffer>>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrices_buffers_;

    void InvokeDrawCall();
    void SetCurrentCamera(const Camera &camera);
    void SetSceneData();
    void Render(const Matrix &view, const Matrix &proj);
    void UpdateBuffer(const Matrix &view, const Matrix &proj);
    void ResizeViewProjMatricesBuffer();
    void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    void DepthRender() const;
    void ExecuteRenderCommands();

public:
    Event<> on_rendering;
    
    static RenderPipeline *Instance();
    static size_t GetRendererCount();
    static std::shared_ptr<Camera> GetCurrentCamera();
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth, const Shader &shader);

    static void Submit(const std::shared_ptr<Mesh> &mesh, std::vector<AssetPtr<Material>> &materials, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address = {}, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle = {});
    static void Submit(AssetPtr<FontData> font_data, Vector2 position, const std::string &string, Color color);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
    static void RequestRender(Camera camera);
};
}