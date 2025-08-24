#pragma once
#include "Texture2D.h"

#include <array>
#include <directx/d3d12.h>

class TextureCube
{
    Microsoft::WRL::ComPtr<ID3D12Resource> m_p_resource_;

public:
    bool CreateTexCube(const std::array<std::shared_ptr<Texture2D>, 6> &textures);

    ID3D12Resource *Resource() const;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() const;
};