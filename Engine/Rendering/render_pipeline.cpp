#include "pch.h"
#include "render_pipeline.h"

#include "application.h"
#include "engine_time.h"
#include "Components/camera_component.h"
#include "Components/renderer.h"
#include "gizmos.h"
#include "lighting.h"
#include "primitives.h"
#include "scene_data.h"
#include "skybox.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/light.h"
#include "raytracing/raytracing_global_root_signature.h"
#include "raytracing/raytracing_pipeline_state.h"

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
        auto world_matrix = renderer->BoundsOrigin();

        BoundingBox world_bounds;
        renderer->bounds.Transform(world_bounds, world_matrix);

        if (frustum.Intersects(world_bounds))
        {
            results.emplace_back(renderer);
        }
    }
    return results;
}

void SortCommands(std::vector<engine::RenderCommand> &render_commands, const Vector3 camera_pos)
{
    for (auto &command : render_commands)
    {
        uint64_t render_queue = 0;
        float depth = 0.0f;
        if (command.type == engine::CommandType::Mesh)
        {
            render_queue = command.mesh_data.material->render_queue;
            depth = (*command.mesh_data.pos - camera_pos).Length();
        }
        else
        {
            render_queue = 8000;
            depth = 0.0f;
        }

        const auto sort_key = engine::RenderPipeline::GenerateSortKey(render_queue, depth, *command.mesh_data.shader);
        command.priority = sort_key;
    }

    std::ranges::sort(render_commands,
                      [](const engine::RenderCommand &a, const engine::RenderCommand &b) {
                          return a.priority < b.priority;
                      });
}
}

namespace engine
{
void RenderPipeline::RenderMainRenderTarget(const std::shared_ptr<CameraComponent> &main_camera)
{
    // store previous property as we're editing aspect ratio to match window aspect ratio
    const auto prev_property = main_camera->property;
    main_camera->property.aspect_ratio = static_cast<float>(Application::WindowWidth()) / static_cast<float>(Application::WindowHeight());
    SetCurrentCamera(main_camera->GetCamera());
    const auto view = main_camera->ViewMatrix();
    const auto proj = main_camera->property.ProjectionMatrix();

    Lighting::Instance()->UpdateLightsViewProjMatrixBuffer(view, proj);
    DepthRender();

    RenderEngine::Instance()->SetMainRenderTarget(main_camera->property.background_color);
    Render(view, proj);

    // revert back to original property
    main_camera->property = prev_property;
}

void RenderPipeline::RenderCamera(const Camera &camera)
{
    ID3D12DescriptorHeap *rtv_heap = nullptr;
    ID3D12DescriptorHeap *dsv_heap = nullptr;

    const auto render_tex = camera.render_texture;
    if (render_tex)
    {
        render_tex->BeginRender(camera.background_color);
        rtv_heap = render_tex->GetHeap();
    }

    const auto depth_tex = camera.depth_texture;
    if (depth_tex)
    {
        depth_tex->BeginRender();
        dsv_heap = depth_tex->GetHeap();
    }

    if (rtv_heap == nullptr && dsv_heap == nullptr)
        return;

    const auto view = camera.view;
    const auto proj = camera.projection;

    SetCurrentCamera(camera);
    Lighting::Instance()->UpdateLightsViewProjMatrixBuffer(view, proj);

    DepthRender();

    RenderEngine::Instance()->SetRenderTarget(rtv_heap, dsv_heap, camera.background_color);
    Render(view, proj);

    if (render_tex)
        render_tex->EndRender();

    if (depth_tex)
        depth_tex->EndRender();
}

void RenderPipeline::RenderVoid()
{
    const auto view = Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
    const auto proj = Matrix::CreatePerspectiveFieldOfView(75 * Mathf::kDeg2Rad, Application::WindowAspectRatio(), 0.1f, 1000.0f);

    SetCurrentCamera(Camera(UINT_MAX, Color(), view, proj, nullptr, nullptr));
    Lighting::Instance()->UpdateLightsViewProjMatrixBuffer(view, proj);
    DepthRender();

    RenderEngine::Instance()->SetMainRenderTarget(Color());
    Render(view, proj);
}

void RenderPipeline::InvokeDrawCall()
{
    for (auto view_proj_matrices_buffers : m_view_proj_matrix_buffers_)
    {
        view_proj_matrices_buffers.ReturnAll();
    }

    const auto cmd_list = RenderEngine::CommandList();
    const auto descriptor_heap = DescriptorHeap::Instance()->GetHeap();
    cmd_list->SetDescriptorHeaps(1, &descriptor_heap);

    RaytracingManager::Instance()->Execute();

    cmd_list->SetGraphicsRootSignature(RootSignature::Get());

    for (const auto camera : m_requesting_cameras_)
    {
        RenderCamera(camera);
    }

    if (const auto main_camera = CameraComponent::Main())
    {
        RenderMainRenderTarget(main_camera);
    }
    else
    {
        RenderVoid();
    }

    on_rendering.Invoke();
    int current_idx_buffer = RenderEngine::CurrentBackBufferIndex();
    m_dynamic_descriptor_heaps_[current_idx_buffer]->Reset();
}

void RenderPipeline::SetCurrentCamera(const Camera &camera)
{
    m_current_camera_ = camera;
}

void RenderPipeline::SetViewProjMatrix(const Matrix &view, const Matrix &proj)
{
    const auto cmd_list = RenderEngine::CommandList();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto view_projection_buffer = *m_view_proj_matrix_buffers_[current_buffer_idx].Get();
    ViewProjection view_projection;
    view_projection.matrices[0] = view;
    view_projection.matrices[1] = proj;
    view_projection_buffer->UpdateBuffer(&view_projection);

    cmd_list->SetGraphicsRootConstantBufferView(kViewProjCBV, view_projection_buffer->GetAddress());
}

void RenderPipeline::SetSceneData()
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
    scene_data.time = Time::Get()->TimeSinceStartUp();
    scene_data.delta_time = Time::GetDeltaTime();

    m_scene_data_buffer_->UpdateBuffer(&scene_data);

    cmd_list->SetGraphicsRootConstantBufferView(kSceneDataCBV, m_scene_data_buffer_->GetAddress());
}

void RenderPipeline::UpdateBuffer(const Matrix &view, const Matrix &proj)
{
    SetViewProjMatrix(view, proj);
    SetSceneData();
    auto lighting_instance = Lighting::Instance();
    lighting_instance->SetLightsViewProjMatrix();
    lighting_instance->SetShadowMap();
    lighting_instance->SetCascadeSlicesBuffer();
    lighting_instance->SetBuffers();
    Skybox::Instance()->Render();
}

void RenderPipeline::Render(const Matrix &view, const Matrix &proj)
{
    UpdateBuffer(view, proj);

    const auto camera_pos = GetCurrentCamera().GetWorldMatrix().Translation();
    auto renderers = FilterVisibleObjects(m_renderers_, view, proj);

    SortCommands(m_commands_, camera_pos);

    ExecuteRenderCommands();
    Gizmos::Render();
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

void RenderPipeline::ExecuteRenderCommands()
{
    const Shader *current_shader = nullptr;
    const Material *current_material = nullptr;
    const Mesh *current_mesh = nullptr;

    auto cmd_list = RenderEngine::CommandList();
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    bool is_sprite_bath_active = false;
    auto sprite_batch = FontData::SpriteBatch();

    for (auto &command : m_commands_)
    {
        if (command.type == CommandType::Mesh)
        {
            if (is_sprite_bath_active)
            {
                sprite_batch->End();
                sprite_batch->End();
                RenderEngine::CommandList()->SetGraphicsRootSignature(RootSignature::Get());
                cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                current_material = nullptr;
                current_shader = nullptr;
                current_mesh = nullptr;
            }

            const auto &[pos, shader, material, mesh, sub_mesh_index, world_address, bone_handle] = command.mesh_data;

            if (mesh == nullptr || shader == nullptr || material == nullptr)
            {
                Logger::Error<RenderPipeline>("render command is invalid!");
                continue;
            }

            if (current_shader != shader)
            {
                current_shader = shader;
                PSOManager::SetPipelineState(cmd_list, current_shader);
            }

            if (current_mesh != mesh)
            {
                current_mesh = mesh;
                if (mesh->vertex_buffer == nullptr)
                    mesh->ReconstructMeshesBuffer();

                if (mesh->vertex_buffer)
                    cmd_list->IASetVertexBuffers(0, 1, mesh->vertex_buffer->View());
            }

            if (world_address != 0)
            {
                cmd_list->SetGraphicsRootConstantBufferView(kWorldCBV, world_address);
            }

            if (bone_handle.ptr != 0)
            {
                cmd_list->SetGraphicsRootDescriptorTable(kBoneSRV, bone_handle);
            }

            if (current_material != material)
            {
                current_material = material;
                if (material->p_shared_material_block == nullptr)
                    material->CreateMaterialBlock();

                material->SetDescriptorTable();
            }

            if (sub_mesh_index == -1)
            {
                cmd_list->IASetIndexBuffer(mesh->index_buffers[0]->View());
                const auto index_count = mesh->HasSubMeshes()
                    ? mesh->sub_meshes[0].base_index
                    : mesh->indices.size();

                cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), 1, 0, 0, 0);
            }
            else
            {
                cmd_list->IASetIndexBuffer(mesh->index_buffers[sub_mesh_index]->View());

                const auto sub_mesh = mesh->sub_meshes[sub_mesh_index];
                cmd_list->DrawIndexedInstanced(sub_mesh.index_count, 1, 0, 0, 0);
            }
        }
        else if (command.type == CommandType::Text)
        {
            if (!is_sprite_bath_active)
            {
                sprite_batch->Begin(RenderEngine::CommandList());
                is_sprite_bath_active = true;

                current_material = nullptr;
                current_shader = nullptr;
                current_mesh = nullptr;
            }

            const auto [font_data, position, string, color] = command.text_data;

            const auto sprite_font = font_data->SpriteFont();

            sprite_font->DrawString(sprite_batch.get(), string, *position, *color);
        }
    }

    if (is_sprite_bath_active)
    {
        sprite_batch->End();
        cmd_list->SetGraphicsRootSignature(RootSignature::Get());
    }

    m_commands_.clear();
}

void RenderPipeline::Submit(const std::shared_ptr<Mesh> &mesh, std::vector<AssetPtr<Material>> &materials, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle)
{
    const auto instance = Instance();

    if (materials.empty())
        return;

    for (auto i = 0; i < materials.size(); ++i)
    {
        const auto casted_material = materials[i].CastedLock();

        if (casted_material == nullptr)
            continue;

        const auto casted_shader = casted_material->shader.CastedLock();
        if (!casted_shader)
        {
            continue;
        }

        RenderCommand cmd;
        cmd.type = CommandType::Mesh;
        cmd.mesh_data.shader = casted_shader.get();
        cmd.mesh_data.material = casted_material.get();
        cmd.mesh_data.pos = &pos;
        cmd.mesh_data.mesh = mesh.get();
        cmd.mesh_data.sub_mesh_index = i - 1;
        cmd.mesh_data.world_matrix_buffer_address = world_matrix_address;
        cmd.mesh_data.bone_matrices_buffer_handle = bone_matrices_handle;

        instance->m_commands_.emplace_back(cmd);
    }
}

void RenderPipeline::Submit(AssetPtr<FontData> font_data, Vector2 position, const std::string &string, Color color)
{
    const auto casted_font_data = font_data.CastedLock();
    if (casted_font_data == nullptr)
        return;

    RenderCommand cmd;
    cmd.type = CommandType::Text;
    cmd.text_data.font_data = casted_font_data.get();
    cmd.text_data.position = &position;
    cmd.text_data.string = string.c_str();
    cmd.text_data.color = &color;

    Instance()->m_commands_.emplace_back(cmd);
}

uint64_t RenderPipeline::GenerateSortKey(const uint64_t render_queue, const float depth, const Shader &shader)
{
    uint64_t key = 0;

    key |= render_queue << 48;

    const auto valid_depth = std::max(0.0f, depth);
    uint32_t dist_bits;
    std::memcpy(&dist_bits, &valid_depth, sizeof(uint32_t));

    const uint64_t shader_id = reinterpret_cast<uintptr_t>(&shader) & 0xFFFFFFFF;

    const bool is_transparent = shader.ShaderSettings().use_blend;

    if (!is_transparent)
    {
        key |= (shader_id & 0xFFFF) << 32;

        key |= static_cast<uint64_t>(dist_bits);
    }
    else
    {
        const uint64_t inverted_depth = ~dist_bits;
        key |= inverted_depth << 16;

        key |= (shader_id & 0xFFFF);
    }

    return key;
}

void RenderPipeline::Init()
{
    const auto instance = Instance();
    for (auto view_proj_matrices_buffers : instance->m_view_proj_matrix_buffers_)
    {
        view_proj_matrices_buffers.SetMaxSize(kStableCameraCount);
    }

    instance->m_commands_.reserve(kReserveRendererCount);

    auto descriptor_heap = DescriptorHeap::Instance()->GetHeap();
    uint32_t start_idx = kStaticDescriptorHeapCount;
    uint32_t capacity = DescriptorHeap::kHandleMax - kStaticDescriptorHeapCount / RenderEngine::kFrame_Buffer_Count;
    uint32_t descriptor_size = DescriptorHeap::Instance()->DescriptorSize();

    instance->m_static_descriptor_heap_ = std::make_shared<SubDescriptorHeap>(descriptor_heap, 0, kStaticDescriptorHeapCount, descriptor_size);

    for (auto &sub_descriptor_heap : instance->m_dynamic_descriptor_heaps_)
    {
        sub_descriptor_heap = std::make_shared<SubDescriptorHeap>(descriptor_heap, start_idx, capacity, descriptor_size);
    }
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

Camera RenderPipeline::GetCurrentCamera()
{
    return Instance()->m_current_camera_;
}

std::shared_ptr<SubDescriptorHeap> RenderPipeline::GetStaticDescriptorHeap()
{
    return Instance()->m_static_descriptor_heap_;
}

std::shared_ptr<SubDescriptorHeap> RenderPipeline::GetDynamicDescriptorHeap()
{
    auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    return Instance()->m_dynamic_descriptor_heaps_[current_buffer_idx];
}

void RenderPipeline::AddRenderer(std::shared_ptr<Renderer> renderer)
{
    const auto mesh_renderer = std::dynamic_pointer_cast<MeshRenderer>(renderer);
    if (mesh_renderer != nullptr)
        RaytracingManager::Instance()->RegisterMeshRenderer(mesh_renderer);
    
    Instance()->m_renderers_.emplace_back(renderer);
}

void RenderPipeline::RemoveRenderer(const std::shared_ptr<Renderer> &renderer)
{
    const auto mesh_renderer = std::dynamic_pointer_cast<MeshRenderer>(renderer);
    if (mesh_renderer != nullptr)
        RaytracingManager::Instance()->UnRegisterMeshRenderer(mesh_renderer);

    auto &renderers = Instance()->m_renderers_;
    std::erase_if(renderers,
                  [&](const auto &r) {
                      return r == renderer;
                  });
}

void RenderPipeline::RequestRender(Camera camera)
{
    Instance()->m_requesting_cameras_.emplace_back(camera);
}
}