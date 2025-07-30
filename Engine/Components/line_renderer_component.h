#pragma once
#include "renderer.h"
#include "Rendering/line_renderer_component.h"

namespace engine
{
class LineRendererComponent : public Renderer
{
    LineRenderer m_LineRenderer;

public:
    void SetVertices(std::vector<Vertex> vertices);
    void SetIndices(std::vector<uint32_t> indices);

    void OnInspectorGui() override;

    void OnDraw() override;

};
}