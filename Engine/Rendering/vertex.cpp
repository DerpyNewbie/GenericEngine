#include "pch.h"

#include "vertex.h"

namespace engine
{
std::shared_ptr<const CRS::VertexLayout> Vertex::Layout()
{
    static const auto layout = [] {
        auto result = std::make_shared<CRS::VertexLayout>();
        result->stride = sizeof(Vertex);
        auto &attributes = result->attributes;
        attributes.push_back({"POSITION", 0, CRS::Format::R32G32B32_Float, offsetof(Vertex, vertex)});
        attributes.push_back({"COLOR", 0, CRS::Format::R32G32B32A32_Float, offsetof(Vertex, color)});
        attributes.push_back({"NORMAL", 0, CRS::Format::R32G32B32_Float, offsetof(Vertex, normal)});
        attributes.push_back({"TANGENT", 0, CRS::Format::R32G32B32A32_Float, offsetof(Vertex, tangent)});
        for (uint32_t i = 0; i < 8; ++i)
        {
            attributes.push_back({
                "TEXCOORD", i, CRS::Format::R32G32_Float,
                static_cast<uint32_t>(offsetof(Vertex, uvs) + sizeof(Vector2) * i)
            });
        }
        attributes.push_back({"BONESPERVERTEX", 0, CRS::Format::R32_UInt, offsetof(Vertex, bones_per_vertex)});
        attributes.push_back({"BONEINDEX", 0, CRS::Format::R32G32B32A32_UInt, offsetof(Vertex, bone_index)});
        attributes.push_back({"BONEWEIGHT", 0, CRS::Format::R32G32B32A32_Float, offsetof(Vertex, bone_weight)});
        return std::shared_ptr<const CRS::VertexLayout>(result);
    }();
    return layout;
}
}
