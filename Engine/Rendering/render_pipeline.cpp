#include "pch.h"
#include "render_pipeline.h"

#include "application.h"
#include "engine_time.h"
#include "Components/camera_component.h"
#include "Components/renderer.h"
#include "gizmos.h"
#include "lighting.h"
#include "scene_data.h"
#include "skybox.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/canvas.h"
#include "Components/light.h"
#include "Components/text_renderer.h"
#include "Effect/effekseer_controller.h"

using namespace DirectX;

namespace
{
std::vector<std::shared_ptr<engine::Renderer>> FilterVisibleObjects(
    const std::vector<std::shared_ptr<engine::Renderer>> &renderers, const Matrix &view, const Matrix &proj)
{
    BoundingFrustum frustum;
    BoundingFrustum::CreateFromMatrix(frustum, proj, true);
    frustum.Transform(frustum, view.Invert());

    std::vector<std::shared_ptr<engine::Renderer>> results;
    for (auto renderer : renderers)
    {
        auto world_matrix = renderer->BoundsOrigin()->WorldMatrix();

        BoundingBox world_bounds;
        renderer->bounds.Transform(world_bounds, world_matrix);

        if (frustum.Intersects(world_bounds))
        {
            results.emplace_back(renderer);
        }
    }
    return results;
}
}

namespace engine
{
void RenderPipeline::InvokeDrawCall()
{
    auto update_speed = 1.0f / Time::GetDeltaTime() / 60.0f;
    EffekseerController::Instance()->m_manager_->Update(update_speed);
    EffekseerController::Instance()->m_memory_pool_->NewFrame();
    
    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootSignature(RootSignature::Get());
    const auto descriptor_heap = DescriptorHeap::GetHeap();
    cmd_list->SetDescriptorHeaps(1, &descriptor_heap);

    for (const auto camera : m_cameras_)
    {
        ID3D12DescriptorHeap *rtv_heap = nullptr;
        ID3D12DescriptorHeap *dsv_heap = nullptr;

        auto render_tex = camera->m_render_texture_.CastedLock();
        if (render_tex)
        {
            render_tex->BeginRender(camera->m_property_.background_color);
            rtv_heap = render_tex->GetHeap();
        }

        auto depth_tex = camera->m_depth_texture_.CastedLock();
        if (depth_tex)
        {
            depth_tex->BeginRender();
            dsv_heap = depth_tex->GetHeap();
        }

        if (rtv_heap == nullptr && dsv_heap == nullptr)
            continue;

        const auto view = camera->ViewMatrix();
        const auto proj = camera->m_property_.ProjectionMatrix();

        CameraComponent::SetCurrentCamera(camera);
        Lighting::Instance()->UpdateLightsViewProjMatrixBuffer(view, proj);

        DepthRender();

        RenderEngine::Instance()->SetRenderTarget(rtv_heap, dsv_heap, camera->m_property_.background_color);
        Render(camera, view, proj);

        if (render_tex)
            render_tex->EndRender();

        if (depth_tex)
            depth_tex->EndRender();
    }

    if (const auto main_camera = CameraComponent::Main())
    {
        // store previous property as we're editing aspect ratio to match window aspect ratio
        const auto prev_property = main_camera->m_property_;
        main_camera->m_property_.aspect_ratio = static_cast<float>(Application::WindowWidth()) / static_cast<float>(Application::WindowHeight());
        CameraComponent::SetCurrentCamera(main_camera);
        const auto view = main_camera->ViewMatrix();
        const auto proj = main_camera->m_property_.ProjectionMatrix();

        Lighting::Instance()->UpdateLightsViewProjMatrixBuffer(view, proj);
        DepthRender();

        RenderEngine::Instance()->SetMainRenderTarget(main_camera->m_property_.background_color);
        Render(main_camera, view, proj);

        // revert back to original property
        main_camera->m_property_ = prev_property;
    }
    else
    {
        const auto view = Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
        const auto proj = Matrix::CreatePerspectiveFieldOfView(75 * Mathf::kDeg2Rad, Application::WindowAspectRatio(), 0.1f, 1000.0f);

        CameraComponent::SetCurrentCamera({});
        Lighting::Instance()->UpdateLightsViewProjMatrixBuffer(view, proj);
        DepthRender();

        RenderEngine::Instance()->SetMainRenderTarget(Color());
        Render(main_camera, view, proj);
    }

    on_rendering.Invoke();
}

void RenderPipeline::SetViewProjMatrix(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj)
{
    if (m_view_proj_matrix_buffers_[camera][0] == nullptr)
    {
        for (auto &view_proj_matrix_buffer : m_view_proj_matrix_buffers_[camera])
        {
            view_proj_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
            view_proj_matrix_buffer->CreateBuffer();
        }
    }

    const auto cmd_list = RenderEngine::CommandList();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto view_projection_buffer = m_view_proj_matrix_buffers_[camera][current_buffer_idx];
    ViewProjection view_projection;
    view_projection.matrices[0] = view;
    view_projection.matrices[1] = proj;
    view_projection_buffer->UpdateBuffer(&view_projection);

    cmd_list->SetGraphicsRootConstantBufferView(kViewProjCBV, view_projection_buffer->GetAddress());
}

void RenderPipeline::SetSceneData(const std::shared_ptr<CameraComponent> &camera)
{
    if (m_scene_data_buffer_ == nullptr)
    {
        m_scene_data_buffer_ = std::make_shared<ConstantBuffer>(sizeof(SceneData));
        m_scene_data_buffer_->CreateBuffer();
    }

    const auto cmd_list = RenderEngine::CommandList();
    SceneData scene_data;
    scene_data.screen_size = Vector2(static_cast<float>(Application::WindowWidth()), static_cast<float>(Application::WindowHeight()));
    scene_data.shadow_map_size = RenderingConstants::kShadowMapSize;
    scene_data.camera_pos = camera != nullptr ? camera->GameObject()->Transform()->Position() : Vector3::Zero;
    scene_data.time = Time::Get()->TimeSinceStartUp();
    scene_data.delta_time = Time::GetDeltaTime();

    m_scene_data_buffer_->UpdateBuffer(&scene_data);

    cmd_list->SetGraphicsRootConstantBufferView(kSceneDataCBV, m_scene_data_buffer_->GetAddress());
}

void RenderPipeline::UpdateBuffer(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj)
{
    SetViewProjMatrix(camera, view, proj);
    SetSceneData(camera);
    auto lighting_instance = Lighting::Instance();
    lighting_instance->SetLightsViewProjMatrix();
    lighting_instance->SetShadowMap();
    lighting_instance->SetCascadeSlicesBuffer();
    lighting_instance->SetBuffers();
    Skybox::Instance()->Render();
}

void RenderPipeline::Render(const std::shared_ptr<CameraComponent> &camera, const Matrix &view, const Matrix &proj)
{
    UpdateBuffer(camera, view, proj);

    auto renderers = FilterVisibleObjects(m_renderers_, view, proj);
    std::ranges::sort(renderers,
                      [](const std::shared_ptr<Renderer> &a, const std::shared_ptr<Renderer> &b) {
                          return a->m_render_queue_ < b->m_render_queue_;
                      });
    for (const auto renderer : renderers)
    {
        renderer->UpdateBuffer();
        renderer->Render();
    }
    EffekseerController::Render(view, proj);

    Gizmos::Render();
    for (const auto &canvas : std::ranges::views::values(Canvas::m_canvasses_))
    {
        canvas->Render();
    }
}

void RenderPipeline::DepthRender() const
{
    if (Lighting::Instance()->m_lights_.empty())
        return;

    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetPipelineState(PSOManager::Get("Depth"));

    Lighting::Instance()->BeginDepthRender();

    Vector2 shadow_map_size = RenderingConstants::kShadowMapSize;
    D3D12_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = shadow_map_size.x;
    viewport.Height = shadow_map_size.y;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissor_rect;
    scissor_rect.left = 0;
    scissor_rect.right = static_cast<LONG>(shadow_map_size.x);
    scissor_rect.top = 0;
    scissor_rect.bottom = static_cast<LONG>(shadow_map_size.y);

    RenderEngine::Instance()->SetRenderTarget(nullptr, Lighting::Instance()->m_dsv_heap_.Get(),
                                              Color(), &viewport, &scissor_rect);

    auto lighting_instance = Lighting::Instance();
    lighting_instance->SetLightsViewProjMatrix();
    lighting_instance->SetBuffers();

    for (const auto renderer : m_renderers_)
    {
        renderer->UpdateBuffer();
        renderer->DepthRender();
    }

    Lighting::Instance()->EndDepthRender();
}

RenderPipeline *RenderPipeline::Instance()
{
    static auto instance = new RenderPipeline;
    return instance;
}

size_t RenderPipeline::GetRendererCount()
{
    return Instance()->m_renderers_.size();
}

void RenderPipeline::AddRenderer(std::shared_ptr<Renderer> renderer)
{
    Instance()->m_renderers_.emplace_back(renderer);
}

void RenderPipeline::RemoveRenderer(const std::shared_ptr<Renderer> &renderer)
{
    auto &renderers = Instance()->m_renderers_;
    std::erase_if(renderers,
                  [&](const auto &r) {
                      return r == renderer;
                  });
}

void RenderPipeline::AddCamera(std::shared_ptr<CameraComponent> camera)
{
    Logger::Log<RenderPipeline>("Adding camera: %s", camera->Name().c_str());
    Instance()->m_cameras_.emplace(camera);
    Instance()->m_view_proj_matrix_buffers_.emplace(camera, std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count>());
}

void RenderPipeline::RemoveCamera(const std::shared_ptr<CameraComponent> &camera)
{
    Logger::Log<RenderPipeline>("Removing camera: %s", camera->Name().c_str());
    auto instance = Instance();
    instance->m_cameras_.erase(camera);
}
}