#include "pch.h"
#include "world_view_projection.h"

WorldViewProjection::WorldViewProjection()
{
    WVP[0] = DirectX::XMMatrixIdentity();
    WVP[1] = DirectX::XMMatrixIdentity();
    WVP[2] = DirectX::XMMatrixIdentity();
}