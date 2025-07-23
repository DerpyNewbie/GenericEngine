#include "FontData.h"
#include "CabotEngine/Graphics/DescriptorHeapManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"

std::shared_ptr<DirectX::GraphicsMemory> engine::FontData::graphics_memory;
std::shared_ptr<DirectX::SpriteBatch> engine::FontData::sprite_batch;

void engine::FontData::Initialize()
{
    auto device = g_RenderEngine->Device();
    graphics_memory = std::make_shared<DirectX::GraphicsMemory>(device);

    DirectX::ResourceUploadBatch resource_upload_batch(device);
    resource_upload_batch.Begin();
    DirectX::RenderTargetState rtState(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT);
    DirectX::SpriteBatchPipelineStateDescription pd(rtState);

    sprite_batch = std::make_shared<DirectX::SpriteBatch>(device, resource_upload_batch, pd);
    sprite_batch->SetViewport(g_RenderEngine->ViewPort());

    auto future = resource_upload_batch.End(g_RenderEngine->CommandQueue());

    g_RenderEngine->WaitRender();
    future.wait();
}

void engine::FontData::LoadFont(const std::wstring &font_path)
{
    auto device = g_RenderEngine->Device();

    DirectX::ResourceUploadBatch resource_upload_batch(device);
    resource_upload_batch.Begin();

    m_SpritefontHandle_ = g_DescriptorHeapManager.Get().Allocate();
    m_Spritefont_ = std::make_shared<DirectX::SpriteFont>(device,
                                                          resource_upload_batch,
                                                          font_path.c_str(),
                                                          m_SpritefontHandle_->HandleCPU,
                                                          m_SpritefontHandle_->HandleGPU);

    auto future = resource_upload_batch.End(g_RenderEngine->CommandQueue());

    g_RenderEngine->WaitRender();
    future.wait();
}