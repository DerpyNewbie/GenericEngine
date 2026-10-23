#include "pch.h"
#include "Rendering/CaboRenderingSystem/Scene/mesh_renderer_data_store.h"

namespace CRS
{
MeshRendererId MeshRendererDataStore::Register(MeshRendererData data)
{
    data.previous_world = data.world;
    return m_data_.Add(std::move(data));
}

void MeshRendererDataStore::Unregister(const MeshRendererId id)
{
    m_data_.Remove(id);
}

bool MeshRendererDataStore::IsValid(const MeshRendererId id) const
{
    return m_data_.Get(id) != nullptr;
}

void MeshRendererDataStore::Update(const MeshRendererId id, MeshRendererData data)
{
    const auto current = m_data_.Get(id);
    if (current == nullptr)
        return;

    data.previous_world = current->world;
    *current = std::move(data);
}

void MeshRendererDataStore::UpdateTransform(const MeshRendererId id, const Matrix &world)
{
    const auto current = m_data_.Get(id);
    if (current == nullptr)
        return;

    current->previous_world = current->world;
    current->world = world;
}

const MeshRendererData *MeshRendererDataStore::Get(const MeshRendererId id) const
{
    return m_data_.Get(id);
}

size_t MeshRendererDataStore::Count() const
{
    return m_data_.Count();
}

std::vector<MeshRendererData> MeshRendererDataStore::TakeSnapshot() const
{
    std::vector<MeshRendererData> snapshot;
    snapshot.reserve(m_data_.Count());
    m_data_.ForEach([&](MeshRendererId, const MeshRendererData &data) {
        snapshot.push_back(data);
    });
    return snapshot;
}
}
