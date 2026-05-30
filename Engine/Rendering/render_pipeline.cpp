#include "pch.h"
#include "render_pipeline.h"

#include "application.h"
#include "compute_command.h"
#include "engine_time.h"
#include "Components/camera_component.h"
#include "Components/renderer.h"
#include "gizmos.h"
#include "gpu_resource_manager.h"
#include "lighting.h"
#include "render_command.h"
#include "scene_data.h"
#include "skybox.h"
#include "view_projection.h"
#include "Asset/asset_database.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/light.h"

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
            auto render_queue = command.mesh_data.material->render_queue;
            auto depth = (*command.mesh_data.pos - camera_pos).Length();
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

bool SetDescriptorTable(const std::shared_ptr<engine::MaterialBlock> &material_block)
{
    const auto resource_group = engine::GpuResourceManager::GetBuffersForMaterial(material_block);
    const auto cmd_list = RenderEngine::CommandList();

    if (!resource_group->UpdateBuffer(material_block))
        return false;
    if (!resource_group->SetBufferToDescriptorTable())
        return false;

    for (int param_i = 0; param_i < engine::kGpuBufferType_Count; ++param_i)
    {
        const auto param_type = static_cast<engine::kGpuUploadType>(param_i);

        if (resource_group->Empty(param_type))
        {
            continue;
        }

        const int root_param_idx = param_i +
                                   engine::RootSignature::kPreDefinedVariableCount;
        const auto itr = resource_group->Begin(param_type);
        const auto desc_handle = itr.handle->handle_gpu;
        cmd_list->SetGraphicsRootDescriptorTable(root_param_idx, desc_handle);
    }
    return true;
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

    if (const auto render_texture_buffer = camera.render_texture ? TextureCollection::GetRenderTexture(camera.render_texture) : nullptr)
    {
        render_texture_buffer->Transition(D3D12_RESOURCE_STATE_RENDER_TARGET);
        rtv_heap = render_texture_buffer->GetHeap();
    }

    if (const auto depth_texture_buffer = camera.depth_texture ? TextureCollection::GetDepthTexture(camera.depth_texture) : nullptr)
    {
        depth_texture_buffer->Transition(D3D12_RESOURCE_STATE_DEPTH_WRITE);
        dsv_heap = depth_texture_buffer->GetHeap();
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
    m_view_proj_matrix_buffers_.ReturnAll();

    SetSceneData();

    
    const auto cmd_list = RenderEngine::CommandList();
    const auto descriptor_heap = DescriptorHeap::GetHeap();
    cmd_list->SetDescriptorHeaps(1, &descriptor_heap);

    ExecuteComputeCommands();
    
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
    m_requesting_cameras_.clear();
}

void RenderPipeline::SetCurrentCamera(const Camera &camera)
{
    m_current_camera_ = camera;
}

void RenderPipeline::SetViewProjMatrix(const Matrix &view, const Matrix &proj)
{
    const auto cmd_list = RenderEngine::CommandList();
    const auto view_projection_buffer = *m_view_proj_matrix_buffers_.Get();
    ViewProjection view_projection;
    view_projection.matrices[0] = view;
    view_projection.matrices[1] = proj;
    view_projection_buffer->UpdateBuffer(&view_projection);

    cmd_list->SetGraphicsRootConstantBufferView(kViewProjCBV, view_projection_buffer->GetAddress());
}

void RenderPipeline::SetSceneData()
{
    if (m_scene_data_buffer_data_ == nullptr)
    {
        m_scene_data_buffer_data_ = std::make_shared<ConstantBufferData>();
        m_scene_data_buffer_data_->AddVector2Data("screen_size");
        m_scene_data_buffer_data_->AddVector2Data("shadow_map_size");
        m_scene_data_buffer_data_->AddFloatData("time");
        m_scene_data_buffer_data_->AddFloatData("delta_time");
    }

    m_scene_data_buffer_data_->SetVector2Data("screen_size", Vector2(static_cast<float>(Application::WindowWidth()), static_cast<float>(Application::WindowHeight())));
    m_scene_data_buffer_data_->SetVector2Data("shadow_map_size", RenderingConstants::kShadowMapSize);
    m_scene_data_buffer_data_->SetFloatData("time", Time::Get()->TimeSinceStartUp());
    m_scene_data_buffer_data_->SetFloatData("delta_time", Time::GetDeltaTime());

    GpuResourceManager::SetGlobalBufferData("SceneData", m_scene_data_buffer_data_);
}

void RenderPipeline::UpdateBuffer(const Matrix &view, const Matrix &proj)
{
    SetViewProjMatrix(view, proj);
    auto lighting_instance = Lighting::Instance();
    lighting_instance->SetLightsViewProjMatrix();
    lighting_instance->SetShadowMap();
    lighting_instance->SetBuffers();
    Skybox::Instance()->Render();
}

void RenderPipeline::Render(const Matrix &view, const Matrix &proj)
{
    UpdateBuffer(view, proj);

    const auto camera_pos = GetCurrentCamera().GetWorldMatrix().Translation();
    auto renderers = FilterVisibleObjects(m_renderers_, view, proj);

    SortCommands(m_render_commands_, camera_pos);

    ExecuteRenderCommands();
    Gizmos::Render();
}

void RenderPipeline::DepthRender()
{
    if (Lighting::Instance()->m_lights_.empty())
        return;

    if (m_depth_shader_ == nullptr)
    {
        m_depth_shader_ = AssetDatabase::GetAsset<Shader>("depth.hlsl").CastedLock();
        if (m_depth_shader_ == nullptr)
        {
            Logger::Error<Gizmos>("Failed to load DepthShader.hlsl");
            return;
        }
    }

    const auto cmd_list = RenderEngine::CommandList();
    PSOManager::SetPipelineState(cmd_list, m_depth_shader_.get(), DXGI_FORMAT_R32_FLOAT, 0);

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

    bool is_sprite_bath_active = false;
    auto sprite_batch = FontData::SpriteBatch();

    for (auto &command : m_render_commands_)
    {
        if (command.type == CommandType::Mesh)
        {
            if (is_sprite_bath_active)
            {
                sprite_batch->End();
                sprite_batch->End();
                RenderEngine::CommandList()->SetGraphicsRootSignature(RootSignature::Get());
                current_material = nullptr;
                current_shader = nullptr;
                current_mesh = nullptr;
            }

            const auto &[pos, shader, material, mesh, sub_mesh_index, instance_count , world_address, bone_handle] = command.mesh_data;

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
            cmd_list->IASetPrimitiveTopology(DX_PrimitiveTopology[shader->ShaderSettings().primitive_topology_type]);
            
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
                if (material->shared_material_block == nullptr)
                    material->CreateMaterialBlock();

                if (!SetDescriptorTable(material->shared_material_block))
                    continue;

                current_material = material;
            }

            if (sub_mesh_index == -1)
            {
                cmd_list->IASetIndexBuffer(mesh->index_buffers[0]->View());
                const auto index_count = mesh->HasSubMeshes()
                    ? mesh->sub_meshes[0].base_index
                    : mesh->indices.size();

                cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), instance_count, 0, 0, 0);
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
        else if (command.type == CommandType::ProceduralMesh)
        {
            if (is_sprite_bath_active)
            {
                sprite_batch->End();
                sprite_batch->End();
                RenderEngine::CommandList()->SetGraphicsRootSignature(RootSignature::Get());
                current_material = nullptr;
                current_shader = nullptr;
                current_mesh = nullptr;
            }

            const auto &[shader, material, vertex_count] = command.procedural_mesh_data;

            if (shader == nullptr || material == nullptr)
            {
                Logger::Error<RenderPipeline>("render command is invalid!");
                continue;
            }

            if (current_shader != shader)
            {
                current_shader = shader;
                PSOManager::SetPipelineState(cmd_list, current_shader);
            }
            cmd_list->IASetPrimitiveTopology(DX_PrimitiveTopology[shader->ShaderSettings().primitive_topology_type]);

            if (current_material != material)
            {
                if (material->shared_material_block == nullptr)
                    material->CreateMaterialBlock();

                if (!SetDescriptorTable(material->shared_material_block))
                    continue;

                current_material = material;
            }

            cmd_list->DrawInstanced(vertex_count, 1, 0, 0);
        }
    }

    if (is_sprite_bath_active)
    {
        sprite_batch->End();
        cmd_list->SetGraphicsRootSignature(RootSignature::Get());
    }

    m_render_commands_.clear();
}

void RenderPipeline::ExecuteComputeCommands()
{
    for (auto &compute_command : m_compute_commands_)
    {
        compute_command.Execute();
    }

    m_compute_commands_.clear();
}

void RenderPipeline::Submit(const std::shared_ptr<Mesh> &mesh, const std::vector<AssetPtr<Material>> &materials, uint32_t instance_count, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle)
{
    const auto instance = Instance();

    if (materials.empty())
        return;

    for (auto i = 0; i < materials.size(); ++i)
    {
        const auto casted_material = materials[i].CastedLock();

        if (casted_material == nullptr)
            continue;

        const auto casted_shader = casted_material->GetShader().CastedLock();
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
        cmd.mesh_data.instance_count = instance_count;
        cmd.mesh_data.world_matrix_buffer_address = world_matrix_address;
        cmd.mesh_data.bone_matrices_buffer_handle = bone_matrices_handle;

        instance->m_render_commands_.emplace_back(cmd);
    }
}

void RenderPipeline::Submit(const AssetPtr<FontData> &font_data, Vector2 position, const std::string &string, Color color)
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

    Instance()->m_render_commands_.emplace_back(cmd);
}

void RenderPipeline::Submit(const std::vector<AssetPtr<Material>> &materials, const uint32_t vertex_count)
{
    const auto instance = Instance();

    if (materials.empty())
        return;

    for (auto i = 0; i < materials.size(); ++i)
    {
        const auto casted_material = materials[i].CastedLock();

        if (casted_material == nullptr)
            continue;

        const auto casted_shader = casted_material->GetShader().CastedLock();
        if (!casted_shader)
        {
            continue;
        }

        RenderCommand cmd;
        cmd.type = CommandType::ProceduralMesh;
        cmd.procedural_mesh_data.shader = casted_shader.get();
        cmd.procedural_mesh_data.material = materials[i].CastedLock().get();
        cmd.procedural_mesh_data.vertex_count = vertex_count;

        instance->m_render_commands_.emplace_back(cmd);
    }
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

void RenderPipeline::Submit(const AssetPtr<ComputeShader> &compute_shader, const std::shared_ptr<MaterialBlock> &material_block, const uint32_t group_count_x, const uint32_t group_count_y, const uint32_t group_count_z)
{
    ComputeCommand cmd(compute_shader, material_block, group_count_x, group_count_y, group_count_z);
    Instance()->m_compute_commands_.emplace_back(cmd);
}

void RenderPipeline::Init()
{
    const auto instance = Instance();

    instance->m_view_proj_matrix_buffers_.SetMaxSize(kStableCameraCount);
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

void RenderPipeline::RequestRender(Camera camera)
{
    Instance()->m_requesting_cameras_.emplace_back(camera);
}
}