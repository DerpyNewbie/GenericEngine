#include "Rendering/CaboRenderingSystem/Scene/light_data_store.h"

namespace CRS
{
LightId LightDataStore::Register(const LightData &data)
{
    return m_lights_.Add(data);
}

void LightDataStore::Unregister(const LightId id)
{
    m_lights_.Remove(id);
}

void LightDataStore::Update(const LightId id, const LightData &data)
{
    if (const auto light = m_lights_.Get(id))
        *light = data;
}

bool LightDataStore::IsValid(const LightId id) const
{
    return m_lights_.Get(id) != nullptr;
}

size_t LightDataStore::Count() const
{
    return m_lights_.Count();
}

std::vector<LightData> LightDataStore::TakeSnapshot() const
{
    std::vector<LightData> snapshot;
    snapshot.reserve(m_lights_.Count());
    m_lights_.ForEach([&](LightId, const LightData &data) {
        snapshot.push_back(data);
    });
    return snapshot;
}
}
