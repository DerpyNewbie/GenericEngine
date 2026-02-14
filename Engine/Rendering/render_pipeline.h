#pragma once
#include "camera.h"
#include "event.h"
#include "material.h"
#include "render_command.h"
#include "render_texture.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{

struct CameraProperty;
class Renderer;
class DepthTexture;

struct RenderRequest
{
    Matrix view;
    Matrix proj;
    Color back_ground_color;
    std::shared_ptr<RenderTexture> render_texture;
    std::shared_ptr<DepthTexture> depth_texture;
};

class RenderPipeline
{
    friend class Light;
    friend class Engine;
    friend class CameraComponent;

    std::vector<RenderCommand> m_commands_;
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::shared_ptr<Camera> m_camera_;
    std::shared_ptr<ConstantBuffer> m_scene_data_buffer_;
    std::vector<RenderRequest> m_render_requests_;
    std::vector<std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count>> m_view_proj_matrix_buffers_;

    void InvokeDrawCall();
    void SetCurrentCamera(const std::shared_ptr<Camera> &camera);
    void SetSceneData(const std::shared_ptr<CameraComponent> &camera);
    void Render(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj);
    void UpdateBuffer(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj);
    void SetViewProjMatrix(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj);
    void DepthRender() const;
    void ExecuteRenderCommands();

public:
    Event<> on_rendering;
    
    static RenderPipeline *Instance();
    static size_t GetRendererCount();
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth, const Shader &shader);

    static void Submit(const std::shared_ptr<Mesh> &mesh, std::vector<AssetPtr<Material>> &materials, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address = {}, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle = {});
    static void Submit(AssetPtr<FontData> font_data, Vector2 position, const std::string &string, Color color);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
    static void AddRenderRequest(const Matrix &view, const Matrix &proj, const std::shared_ptr<RenderTexture> &render_texture = nullptr, Color back_ground_color = Color(), std::shared_ptr<DepthTexture> depth_texture = nullptr);
};
}