#pragma once
#include "Object.h"

class CabotCamera : public engine::Object
{
public:
    CabotCamera();
    void SetPosAndLookAt(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt);

private:
    DirectX::XMMATRIX m_ViewMatrix;
    DirectX::XMMATRIX m_ProjectionMatrix;
};