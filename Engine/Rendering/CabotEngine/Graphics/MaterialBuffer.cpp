#include "pch.h"

#include "MaterialBuffer.h"

#include "DescriptorHeapManager.h"

void MaterialBuffer::Initialize(CabotMaterial &material)
{
    auto values = material.Values();
    auto textures = material.Textures();

    if (!values.empty())
    {
        m_ValuesBuffer.Initialize(values.size());
        m_ValuesBuffer.Upload(values);
        Handle = g_DescriptorHeapManager->Get().Register(m_ValuesBuffer);
    }

    for (const auto &texture : textures)
    {
        if (!Handle)
        {
            Handle = g_DescriptorHeapManager->Get().Register(texture);
        }
        else
        {
            g_DescriptorHeapManager->Get().Register(texture);
        }
    }
}