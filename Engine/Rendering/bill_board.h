#pragma once
#include "pch.h"
#include "event_receivers.h"
#include "object.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/IndexBuffer.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/VertexBuffer.h"

class BillBoard : public IDrawCallReceiver, public engine::Object
{
    std::shared_ptr<engine::VertexBuffer> m_vertex_buffer_;
    std::shared_ptr<engine::IndexBuffer> m_index_buffer_;
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::FRAME_BUFFER_COUNT> m_wvp_buffers_;

public:
    Matrix world_matrix = DirectX::XMMatrixIdentity();

    void OnConstructed() override;
    void OnDraw() override;
};