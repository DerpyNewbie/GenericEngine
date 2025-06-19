#pragma once
#include "object.h"

namespace engine
{
class Texture2D : public Object
{
    std::vector<Color> m_pixels_;
    int m_width_ = 0;
    int m_height_ = 0;

public:
    int Width() const;
    int Height() const;
    const std::vector<Color> &Pixels() const;

    void FillPixel(Color color);
    void SetPixel(int x, int y, Color color);
    void Resize(int width, int height);

    static std::shared_ptr<Texture2D> GetColoredTexture(Color color);
};

}