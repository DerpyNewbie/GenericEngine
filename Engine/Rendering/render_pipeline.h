#pragma once
#include "event.h"
#include "material.h"
#include "render_command.h"
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

    std::unordered_set<std::shared_ptr<CameraComponent>> m_cameras_;
    std::map<std::shared_ptr<CameraComponent>, std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count>> m_view_proj_matrix_buffers_;

    void InvokeDrawCall();
    void SetViewProjMatrix(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj);
    void SetSceneData(const std::shared_ptr<CameraComponent> &camera);
    void UpdateBuffer(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj);
    void Render(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj);
    void DepthRender() const;
    void ExecuteRenderCommands();

public:
    Event<> on_rendering;


    static RenderPipeline *Instance();
    static size_t GetRendererCount();
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth, const Shader &shader);

    static void Submit(const std::shared_ptr<Mesh> &mesh, std::vector<AssetPtr<Material>> &materials, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address = {}, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle = {});
    static void Submit(AssetPtr<FontData> font_data, Vector2 position, const std::string &string, Color color);
    static void AddCamera(std::shared_ptr<CameraComponent> camera);
    static void RemoveCamera(const std::shared_ptr<CameraComponent> &camera);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
};
}