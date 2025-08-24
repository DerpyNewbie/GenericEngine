#pragma once
#include "pch.h"
#include "ComPtr.h"
#include "Texture2D.h"
#include "Asset/asset_ptr.h"

#include <array>
#include <directx/d3d12.h>

class TextureCube
{
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource_;

public:
    bool CreateTexCube(const std::array<std::shared_ptr<Texture2D>, 6> &textures);

    ID3D12Resource *Resource();
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();
};