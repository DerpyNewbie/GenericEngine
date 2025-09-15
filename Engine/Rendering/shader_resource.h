#pragma once
#include "Rendering/ibuffer.h"

namespace engine
{
class ShaderResource : public IBuffer
{
public:
    [[nodiscard]] virtual ID3D12Resource *Resource() = 0;
    [[nodiscard]] virtual D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() = 0;
};
}