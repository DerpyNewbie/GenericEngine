#pragma once
#include "raytracing_shader.h"
#include "Asset/asset_ptr.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
class RaytracePass
{
    AssetPtr<RaytracingShader> m_raytrace_shader_;
    std::shared_ptr<StructuredBuffer> m_materials_buffer_;


};
}