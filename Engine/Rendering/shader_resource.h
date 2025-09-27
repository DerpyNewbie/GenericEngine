#pragma once
#include "CabotEngine/Graphics/DescriptorHeap.h"

namespace engine
{
class ShaderResource
{
public:
    virtual ~ShaderResource() = default;

    [[nodiscard]] virtual ID3D12Resource *Resource() = 0;
    [[nodiscard]] virtual D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() = 0;
};
}