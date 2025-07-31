#include "pch.h"
#include "font_data.h"

#include "CabotEngine/Graphics/DescriptorHeap.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
std::shared_ptr<DirectX::GraphicsMemory> FontData::m_graphics_memory_;
std::shared_ptr<DirectX::SpriteBatch> FontData::m_sprite_batch_;

FontData::FontData()
{
    auto device = g_RenderEngine->Device();
    m_graphics_memory_ = std::make_shared<DirectX::GraphicsMemory>(device);

    DirectX::ResourceUploadBatch resource_upload_batch(device);
    resource_upload_batch.Begin();
    DirectX::RenderTargetState rtState(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT);
    DirectX::SpriteBatchPipelineStateDescription pd(rtState);

    m_sprite_batch_ = std::make_shared<DirectX::SpriteBatch>(device, resource_upload_batch, pd);
    m_sprite_batch_->SetViewport(g_RenderEngine->ViewPort());

    auto future = resource_upload_batch.End(g_RenderEngine->CommandQueue());
    future.wait();
}

void FontData::LoadFont(const std::wstring &font_path)
{
    auto device = g_RenderEngine->Device();

    DirectX::ResourceUploadBatch resource_upload_batch(device);
    resource_upload_batch.Begin();

    m_spritefont_handle_ = DescriptorHeap::Allocate();
    m_spritefont_ = std::make_shared<DirectX::SpriteFont>(device,
                                                          resource_upload_batch,
                                                          font_path.c_str(),
                                                          m_spritefont_handle_->HandleCPU,
                                                          m_spritefont_handle_->HandleGPU);

    auto future = resource_upload_batch.End(g_RenderEngine->CommandQueue());

    g_RenderEngine->WaitRender();
    future.wait();
}
}