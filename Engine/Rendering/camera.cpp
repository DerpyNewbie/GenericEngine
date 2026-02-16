#include "pch.h"
#include "camera.h"

namespace engine
{
Matrix Camera::GetWorldMatrix() const
{
    return view.Invert();
}
}