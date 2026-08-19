#pragma once
#include <ThirdParty/Effekseer/include/Effekseer/Effekseer.h>
#include <ThirdParty/Effekseer/include/EffekseerRendererDX12/EffekseerRendererDX12.h>

#include "Rendering/CabotEngine/Graphics/RenderEngine.h"


namespace engine
{
class EffekseerController
{
    friend class Engine;
    friend class RenderPipeline;
    friend RenderEngine;

    static constexpr float kMaxInstanceCount = 2000;
    static constexpr float kMaxSquareCount = 8000;
    Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> m_memory_pool_;
    Effekseer::RefPtr<EffekseerRenderer::CommandList> m_effekseer_command_list_;
    EffekseerRenderer::RendererRef m_renderer_;
    Effekseer::ManagerRef m_manager_;

    Matrix m_view_;
    Matrix m_proj_;

    static EffekseerController *Instance();
    static void Init();
    static void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    static void Render();

public:
    static Effekseer::ManagerRef Manager();

};
}