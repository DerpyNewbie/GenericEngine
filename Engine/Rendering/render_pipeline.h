#pragma once
#include "event.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"

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

    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::unordered_set<std::shared_ptr<CameraComponent>> m_cameras_;
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_;

    void InvokeDrawCall();
    void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    void UpdateBuffer(const Matrix &view, const Matrix &proj);
    void Render(const std::shared_ptr<CameraComponent> &camera);
    void DepthRender();

public:
    Event<> on_rendering;

    static RenderPipeline *Instance();
    static size_t GetRendererCount();

    static void AddCamera(std::shared_ptr<CameraComponent> camera);
    static void RemoveCamera(const std::shared_ptr<CameraComponent> &camera);
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
};
}