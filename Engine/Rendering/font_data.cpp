#include "pch.h"
#include "font_data.h"

#include "CabotEngine/Graphics/DescriptorHeap.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
std::shared_ptr<DirectX::GraphicsMemory> FontData::graphics_memory;
std::shared_ptr<DirectX::SpriteBatch> FontData::sprite_batch;

FontData::FontData()
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
    future.wait();
}

void FontData::LoadFont(const std::wstring &font_path)
{
    auto device = g_RenderEngine->Device();

    DirectX::ResourceUploadBatch resource_upload_batch(device);
    resource_upload_batch.Begin();

    m_SpritefontHandle_ = DescriptorHeap::Allocate();
    m_Spritefont_ = std::make_shared<DirectX::SpriteFont>(device,
                                                          resource_upload_batch,
                                                          font_path.c_str(),
                                                          m_SpritefontHandle_->HandleCPU,
                                                          m_SpritefontHandle_->HandleGPU);

    auto future = resource_upload_batch.End(g_RenderEngine->CommandQueue());

    g_RenderEngine->WaitRender();
    future.wait();
}
}