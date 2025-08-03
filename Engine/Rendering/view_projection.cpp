#include "pch.h"
#include "view_projection.h"

namespace engine
{
ViewProjection::ViewProjection()
{
    matrices[0] = DirectX::XMMatrixIdentity();
    matrices[1] = DirectX::XMMatrixIdentity();
}
}