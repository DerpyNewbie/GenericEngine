#include "Rendering/CaboRenderingSystem/Scene/camera_data_store.h"

namespace CRS
{
CameraId CameraDataStore::Create(std::unique_ptr<ICameraSettings> settings)
{
    Entry entry;
    entry.settings = std::move(settings);
    return m_cameras_.Add(std::move(entry));
}

void CameraDataStore::Destroy(const CameraId id)
{
    m_cameras_.Remove(id);
}

void CameraDataStore::Update(const CameraId id, const CameraCommon &common)
{
    const auto entry = m_cameras_.Get(id);
    if (entry == nullptr)
        return;

    const auto previous_view_projection = entry->has_previous
                                              ? entry->common.view * entry->common.projection
                                              : common.view * common.projection;
    entry->common = common;
    entry->common.previous_view_projection = previous_view_projection;
    entry->has_previous = true;
}

const CameraCommon *CameraDataStore::GetCommon(const CameraId id) const
{
    const auto entry = m_cameras_.Get(id);
    return entry ? &entry->common : nullptr;
}

ICameraSettings *CameraDataStore::GetSettings(const CameraId id)
{
    const auto entry = m_cameras_.Get(id);
    return entry ? entry->settings.get() : nullptr;
}

std::vector<CameraSnapshot> CameraDataStore::TakeSnapshot() const
{
    std::vector<CameraSnapshot> snapshot;
    snapshot.reserve(m_cameras_.Count());
    m_cameras_.ForEach([&](CameraId, const Entry &entry) {
        // 一度もUpdateされていないカメラは描かない
        if (!entry.has_previous)
            return;

        snapshot.push_back({entry.common, entry.settings ? entry.settings->Clone() : nullptr});
    });
    return snapshot;
}
}
