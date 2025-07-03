#pragma once
#include "ComPtr.h"
#include <directx/d3dx12.h>
#include <string>
#include <memory>
#include <directxtk12/SimpleMath.h>

struct aiTexture;
class DescriptorHeap;
class DescriptorHandle;

class Texture2D
{
public:
    std::vector<DirectX::PackedVector::XMCOLOR> tex_data;
    UINT width;
    UINT height;

    void CreateBuffer();
    
    explicit Texture2D(aiTexture *src);
    static std::shared_ptr<Texture2D> Get(std::string path);
    static std::shared_ptr<Texture2D> Get(std::wstring path);
    static std::shared_ptr<Texture2D> GetWhite();
    static Texture2D *CreateGrayGradationTexture();
    bool IsValid();

    ID3D12Resource *Resource();
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();

    explicit Texture2D(ID3D12Resource *buffer);
    explicit Texture2D(std::wstring path);

    Texture2D(const Texture2D &) = delete;
    void operator =(const Texture2D &) = delete;

private:
    explicit Texture2D(std::string path);
    bool m_IsValid;
    ComPtr<ID3D12Resource> m_pResource;
    bool Load(std::string &path);
    bool Load(std::wstring &path);

    static ID3D12Resource *GetDefaultResource(size_t width, size_t height);
};