#include "pch.h"

#include "Texture2D.h"

#include <cassert>
#include <DirectXTex.h>
#include <assimp/texture.h>
#include <wrl.h>
#include <directx/d3dx12.h>
#include <wincodec.h>

#include "RenderEngine.h"

#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

/* std::string(マルチバイト文字列)からstd::wstring(ワイド文字列)を得る。AssimpLoaderと同じものだけど、共用にするのがめんどくさかったので許してください */
std::wstring GetWideString(const std::string &str)
{
    auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

    std::wstring wstr;
    wstr.resize(num1);

    auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

    assert(num1 == num2);
    return wstr;
}

// return file ext
std::wstring FileExtension(const std::wstring &path)
{
    auto idx = path.rfind(L'.');
    return path.substr(idx + 1, path.length() - idx - 1);
}

Texture2D::Texture2D(std::string path)
{
    m_IsValid = Load(path);
}

Texture2D::Texture2D(std::wstring path)
{
    m_IsValid = Load(path);
}

Texture2D::Texture2D(aiTexture *src)
{
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;

    ComPtr<IWICImagingFactory> wicFactory;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&wicFactory));

    ComPtr<IWICStream> stream;
    wicFactory->CreateStream(&stream);
    stream->InitializeFromMemory(reinterpret_cast<BYTE *>(src->pcData), src->mWidth);

    wicFactory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    decoder->GetFrame(0, &frame);

    ComPtr<IWICFormatConverter> converter;
    wicFactory->CreateFormatConverter(&converter);

    converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom
        );

    UINT width, height;
    frame->GetSize(&width, &height);

    std::vector<uint8_t> imageRGBA(width * height * 4);
    converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(imageRGBA.size()), imageRGBA.data());

    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
                                             width,
                                             height,
                                             1,
                                             1);

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

    auto hr = g_RenderEngine->Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_pResource)
        );

    if (FAILED(hr))
    {
        return;
    }

    D3D12_BOX destRegion = {0, 0, 0, width, height, 1};
    hr = m_pResource->WriteToSubresource(0,
                                         &destRegion, // copy all
                                         imageRGBA.data(), // origin data addr
                                         width * 4, // 1 line size
                                         width * height * 4 // all line sizes
        );

    if (FAILED(hr))
    {
        return;
    }

    m_IsValid = true;
    return;
}

Texture2D::Texture2D(ID3D12Resource *buffer)
{
    m_pResource = buffer;
    m_IsValid = m_pResource != nullptr;
}

bool Texture2D::Load(std::string &path)
{
    auto wpath = GetWideString(path);
    return Load(wpath);
}

bool Texture2D::Load(std::wstring &path)
{
    // WIC texture loading
    TexMetadata meta = {};
    ScratchImage scratch = {};
    auto ext = FileExtension(path);

    HRESULT hr = S_FALSE;
    if (ext == L"png") // use WICFile when png
    {
        hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &meta, scratch);
    }
    else if (ext == L"tga") // use TGAFile when tga
    {
        hr = LoadFromTGAFile(path.c_str(), &meta, scratch);
    }

    if (FAILED(hr))
    {
        return false;
    }

    auto img = scratch.GetImage(0, 0, 0);
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(meta.format,
                                             meta.width,
                                             meta.height,
                                             static_cast<UINT16>(meta.arraySize),
                                             static_cast<UINT16>(meta.mipLevels));

    // create resource
    hr = g_RenderEngine->Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
        );

    if (FAILED(hr))
    {
        return false;
    }

    hr = m_pResource->WriteToSubresource(0,
                                         nullptr, // copy all
                                         img->pixels, // data origin addr
                                         static_cast<UINT>(img->rowPitch), // 1-line size
                                         static_cast<UINT>(img->slicePitch) // all line size?
        );
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

std::shared_ptr<Texture2D> Texture2D::Get(std::string path)
{
    auto wpath = GetWideString(path);
    return Get(wpath);
}

std::shared_ptr<Texture2D> Texture2D::Get(std::wstring path)
{
    auto tex = std::make_shared<Texture2D>(path);
    if (!tex->IsValid())
    {
        return GetWhite(); // return white texture if loading has failed
    }
    return tex;
}

std::shared_ptr<Texture2D> Texture2D::GetWhite()
{
    ID3D12Resource *buff = GetDefaultResource(4, 4);

    std::vector<unsigned char> data(4 * 4 * 4);
    std::fill(data.begin(), data.end(), 0x00);

    auto hr = buff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
    if (FAILED(hr))
    {
        return nullptr;
    }

    return std::make_shared<Texture2D>(buff);
}

Texture2D *Texture2D::CreateGrayGradationTexture()
{
    // D3D12_RESOURCE_DESC resDesc = {};
    // resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // resDesc.Width = 4;
    // resDesc.Height = 256;
    std::vector<unsigned int> data(4 * 256);
    auto it = data.begin();
    unsigned int c = 0xff;
    for (; it != data.end(); it += 4)
    {
        auto col = (0xff << 24) | RGB(c, c, c);
        std::fill(it, it + 4, col);
        --c;
    }

    ID3D12Resource *buff = GetDefaultResource(4, 256);
    auto hr = buff->WriteToSubresource(0, nullptr, data.data(), 4 * sizeof(unsigned int),
                                       sizeof(unsigned int) * data.size());
    if (FAILED(hr))
    {
        return nullptr;
    }
    return new Texture2D(buff);
}

ID3D12Resource *Texture2D::GetDefaultResource(size_t width, size_t height)
{
    auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, width, height);
    auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    ID3D12Resource *buff = nullptr;
    auto result = g_RenderEngine->Device()->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&buff)
        );
    if (FAILED(result))
    {
        assert(false);
        return nullptr;
    }
    return buff;
}

bool Texture2D::IsValid()
{
    return m_IsValid;
}

ID3D12Resource *Texture2D::Resource()
{
    return m_pResource.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = m_pResource->GetDesc().Format;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D texture
    desc.Texture2D.MipLevels = 1; // no mipmaps
    return desc;
}