#pragma once
#include <directxtk12/SpriteFont.h>
#include <directxtk12/ResourceUploadBatch.h>

class DescriptorHandle;

namespace engine
{
class FontData : public Object
{
    static std::shared_ptr<DirectX::GraphicsMemory> graphics_memory;
    static std::shared_ptr<DirectX::SpriteBatch> sprite_batch;
    std::shared_ptr<DirectX::SpriteFont> m_Spritefont_;
    std::shared_ptr<DescriptorHandle> m_SpritefontHandle_;

public:
    static std::shared_ptr<DirectX::GraphicsMemory> GraphicsMemory()
    {
        return graphics_memory;
    }

    static std::shared_ptr<DirectX::SpriteBatch> SpriteBatch()
    {
        return sprite_batch;
    }

    std::shared_ptr<DirectX::SpriteFont> SpriteFont()
    {
        return m_Spritefont_;
    }

    std::shared_ptr<DescriptorHandle> SpritefontHandle()
    {
        return m_SpritefontHandle_;
    }


    FontData();
    void LoadFont(const std::wstring &font_path);
};
}