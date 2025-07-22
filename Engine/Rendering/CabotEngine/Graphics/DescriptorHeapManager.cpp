#include "pch.h"

#include "DescriptorHeapManager.h"

DescriptorHeapManager g_DescriptorHeapManager;

DescriptorHeapManager::DescriptorHeapManager()
{
}

void DescriptorHeapManager::Initialize()
{
    m_DescriptorHeap = std::make_unique<DescriptorHeap>();
}