#pragma once
#include "../../Object.h"

class Camera: public Object
{
public:
    Camera();
    void SetPosAndLookAt(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt);
    
private:
    DirectX::XMMATRIX m_ViewMatrix;
    DirectX::XMMATRIX m_ProjectionMatrix;
};
