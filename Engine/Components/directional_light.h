#pragma once
#include "component.h"
#include "light.h"
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"

namespace engine
{
class DirectionalLight : public Light
{

public:
    void OnAwake() override;
    void OnUpdate() override;
};
}