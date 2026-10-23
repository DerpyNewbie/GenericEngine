#include "pch.h"
#include "Rendering/CaboRenderingSystem/render_engine.h"

#include "Rendering/CaboRenderingSystem/Core/log.h"
#include "Rendering/CaboRenderingSystem/Pipeline/Forward/forward_pipeline.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_render_device.h"
#include "Rendering/CaboRenderingSystem/Scene/frame_snapshot.h"

namespace CRS
{
RenderEngine::RenderEngine(std::unique_ptr<IRenderDevice> device, std::unique_ptr<IRenderPipeline> pipeline,
                           const uint32_t width, const uint32_t height) :
    m_device_(std::move(device)), m_pipeline_(std::move(pipeline)), m_width_(width), m_height_(height)
{
    CreateMainDepth();
}

RenderEngine::~RenderEngine()
{
    if (m_device_)
        m_device_->WaitIdle();

    // ラッパーとパイプラインを先に破棄し、デバイスを最後に破棄する
    m_main_depth_.Reset();
    m_pipeline_.reset();
}

void RenderEngine::CreateMainDepth()
{
    TextureDesc desc;
    desc.width = m_width_;
    desc.height = m_height_;
    desc.format = Format::D32_Float;
    desc.usage = TextureUsage::DepthStencil;
    desc.clear_depth = 1.0f;
    desc.debug_name = "CRS MainDepth";
    m_main_depth_ = Texture(*m_device_, desc);
}

CameraId RenderEngine::CreateCamera()
{
    return m_cameras_.Create(m_pipeline_->CreateCameraSettings());
}

void RenderEngine::DestroyCamera(const CameraId id)
{
    m_cameras_.Destroy(id);
}

MaterialId RenderEngine::CreateDefaultMaterial()
{
    return m_materials_.Add(m_pipeline_->CreateDefaultMaterial());
}

void RenderEngine::Resize(const uint32_t width, const uint32_t height)
{
    if (width == 0 || height == 0)
        return;

    m_width_ = width;
    m_height_ = height;
    m_device_->Resize(width, height);
    CreateMainDepth();
}

void RenderEngine::BeginFrame()
{
    m_device_->BeginFrame();
    m_command_list_ = &m_device_->BeginCommandList();
}

ICommandList &RenderEngine::CommandList()
{
    return *m_command_list_;
}

void RenderEngine::Render(const float time, const float delta_time)
{
    if (m_command_list_ == nullptr)
    {
        LogError("RenderEngine::Render called outside of BeginFrame / EndFrame");
        return;
    }

    FrameSnapshot frame;
    frame.meshes = m_meshes_.TakeSnapshot();
    frame.cameras = m_cameras_.TakeSnapshot();
    frame.lights = m_lights_.TakeSnapshot();
    frame.environment = m_environment_;
    frame.immediate = &m_immediate_;
    frame.time = time;
    frame.delta_time = delta_time;

    m_pipeline_->Render(frame, *m_command_list_, m_materials_);
}

void RenderEngine::EndFrame()
{
    if (m_command_list_ == nullptr)
        return;

    m_command_list_->Transition(m_device_->GetBackBuffer(), ResourceState::Present);
    m_device_->Submit(*m_command_list_);
    m_command_list_ = nullptr;

    m_device_->EndFrame();
    m_device_->Present();

    m_immediate_.Clear();
}

void RenderEngine::RenderFrame(const float time, const float delta_time)
{
    BeginFrame();
    Render(time, delta_time);
    EndFrame();
}

std::unique_ptr<RenderEngine> CreateDefaultRenderEngine(void *native_window, const uint32_t width,
                                                        const uint32_t height)
{
    auto device = D3D12RenderDevice::Create(static_cast<HWND>(native_window), width, height);
    if (device == nullptr)
        return nullptr;

    auto pipeline = std::make_unique<ForwardPipeline>(*device);
    return std::make_unique<RenderEngine>(std::move(device), std::move(pipeline), width, height);
}
}
