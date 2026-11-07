#include "pch.h"
#include "skybox.h"

namespace engine
{
std::shared_ptr<Skybox> Skybox::Instance()
{
    static std::shared_ptr<Skybox> instance(new Skybox());
    return instance;
}

std::shared_ptr<TextureCube> Skybox::TextureCube() const
{
    return m_texture_cube_;
}

void Skybox::SetTextureCube(const std::shared_ptr<class TextureCube> &texture_cube)
{
    m_texture_cube_ = texture_cube;
}
}
