#include "pch.h"

#include "CabotCamera.h"
#include "application.h"

void CabotCamera::SetPosAndLookAt(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt)
{
    auto aspect = static_cast<float>(Application::WindowWidth()) / static_cast<float>(
                      Application::WindowHeight()); // アスペクト比
    auto fov = DirectX::XMConvertToRadians(60);
    auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_ViewMatrix = DirectX::XMMatrixLookAtRH(position, lookAt, upward);
    m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
}