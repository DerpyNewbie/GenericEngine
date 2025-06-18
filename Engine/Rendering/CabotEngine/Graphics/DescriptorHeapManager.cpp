#include "DescriptorHeapManager.h"

DescriptorHeapManager* g_DescriptorHeapManager;

DescriptorHeapManager::DescriptorHeapManager()
{
    m_DescriptorHeap = std::make_unique<DescriptorHeap>();
}
