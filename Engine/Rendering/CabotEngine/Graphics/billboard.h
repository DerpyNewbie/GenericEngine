#pragma once
#include "pch.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "RenderEngine.h"
#include "VertexBuffer.h"

class Billboard
{
public:
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::FRAME_BUFFER_COUNT> wvp_buffers;
    std::shared_ptr<engine::VertexBuffer> vertex_buffer;
    std::shared_ptr<engine::IndexBuffer> index_buffer;
    Matrix world_matrix = DirectX::XMMatrixIdentity();

    void Update();
};