#pragma once
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"

class DescriptorHeapManager
{
public:
    DescriptorHeapManager();
    DescriptorHeap& Get(){return *m_DescriptorHeap;}
private:
    std::unique_ptr<DescriptorHeap> m_DescriptorHeap;
};

extern DescriptorHeapManager* g_DescriptorHeapManager;