#include "texture2d.h"

#include "Math/color.h"

namespace engine
{
int Texture2D::Width() const
{
    return m_width_;
}
int Texture2D::Height() const
{
    return m_height_;
}
const std::vector<Color> &Texture2D::Pixels() const
{
    return m_pixels_;
}
void Texture2D::FillPixel(Color color)
{
    std::fill(m_pixels_.begin(), m_pixels_.end(), color);
}
void Texture2D::SetPixel(const int x, const int y, const Color color)
{
    if (x < 0 || x >= m_width_ || y < 0 || y >= m_height_)
    {
        throw std::out_of_range("Texture2D::SetPixel: x or y out of range");
    }

    m_pixels_[y * m_width_ + x] = color;
}
void Texture2D::Resize(const int width, const int height)
{
    // FIXME: implement proper texture resizing
    m_width_ = width;
    m_height_ = height;
    m_pixels_.resize(width * height);
}
std::shared_ptr<Texture2D> Texture2D::GetColoredTexture(const Color color)
{
    auto texture = Instantiate<Texture2D>("generated_colored_texture");
    texture->Resize(2, 2);
    texture->FillPixel(color);
    return texture;
}
}