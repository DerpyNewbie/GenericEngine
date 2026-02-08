#include "pch.h"
#include "effekseer_controller.h"

#include "effekseer_util.h"
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
EffekseerController *EffekseerController::Instance()
{
    static EffekseerController instance;
    return &instance;
}

void EffekseerController::Init()
{
    const auto instance = Instance();
    auto rtv_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    const auto device = RenderEngine::Device();
    const auto cmd_queue = RenderEngine::CommandQueue();

    auto graphicsDevice = EffekseerRendererDX12::CreateGraphicsDevice(device, cmd_queue, RenderEngine::kFrame_Buffer_Count);
    instance->m_renderer_ = EffekseerRendererDX12::Create(graphicsDevice, &rtv_format, 1, DXGI_FORMAT_D32_FLOAT, false, kMaxSquareCount);

    instance->m_memory_pool_ = EffekseerRenderer::CreateSingleFrameMemoryPool(instance->m_renderer_->GetGraphicsDevice());
    instance->m_effekseer_command_list_ = EffekseerRenderer::CreateCommandList(instance->m_renderer_->GetGraphicsDevice(), instance->m_memory_pool_);

    instance->m_manager_ = Effekseer::Manager::Create(kMaxInstanceCount);
    instance->m_manager_->SetSpriteRenderer(instance->m_renderer_->CreateSpriteRenderer());
    instance->m_manager_->SetRibbonRenderer(instance->m_renderer_->CreateRibbonRenderer());
    instance->m_manager_->SetRingRenderer(instance->m_renderer_->CreateRingRenderer());
    instance->m_manager_->SetTrackRenderer(instance->m_renderer_->CreateTrackRenderer());
    instance->m_manager_->SetModelRenderer(instance->m_renderer_->CreateModelRenderer());

    instance->m_manager_->SetTextureLoader(instance->m_renderer_->CreateTextureLoader());
    instance->m_manager_->SetModelLoader(instance->m_renderer_->CreateModelLoader());
    instance->m_manager_->SetMaterialLoader(instance->m_renderer_->CreateMaterialLoader());
}

void EffekseerController::Render(const Matrix &view, const Matrix &proj)
{
    const auto instance = Instance();
    const auto cmd_list = RenderEngine::CommandList();

    EffekseerRendererDX12::BeginCommandList(instance->m_effekseer_command_list_, cmd_list);
    instance->m_renderer_->SetCommandList(instance->m_effekseer_command_list_);

    instance->m_renderer_->SetCameraMatrix(EffekseerUtil::ToMatrix44(view));
    instance->m_renderer_->SetProjectionMatrix(EffekseerUtil::ToMatrix44(proj));

    instance->m_renderer_->BeginRendering();
    instance->m_manager_->Draw();
    instance->m_renderer_->EndRendering();

    instance->m_renderer_->SetCommandList(nullptr);
    EffekseerRendererDX12::EndCommandList(instance->m_effekseer_command_list_);

    cmd_list->SetGraphicsRootSignature(RootSignature::Get());
    const auto descriptor_heap = DescriptorHeap::GetHeap();
    cmd_list->SetDescriptorHeaps(1, &descriptor_heap);

}

Effekseer::ManagerRef EffekseerController::Manager()
{
    return Instance()->m_manager_;
}
}