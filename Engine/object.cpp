#include "pch.h"

#include "update_manager.h"

namespace
{
std::vector<std::shared_ptr<engine::Object>> g_destroying_objects;
std::vector<std::shared_ptr<engine::Object>> g_destroyed_objects;
}

namespace engine
{

bool Object::m_in_gc_time_ = false;
unsigned int Object::m_last_instantiated_name_count_ = 0;
unsigned int Object::m_last_immediately_destroyed_objects_ = 0;
std::unordered_map<xg::Guid, std::shared_ptr<Object>> Object::m_objects_;

void Object::GarbageCollect()
{
    m_in_gc_time_ = true;
    g_destroyed_objects.clear();
    while (!g_destroying_objects.empty())
    {
        auto destroying_objects = g_destroying_objects;
        g_destroying_objects.clear();
        for (const auto &obj : destroying_objects)
        {
            obj->OnDestroy();
            m_objects_.erase(obj->m_guid_);
            g_destroyed_objects.emplace_back(obj);
        }
    }

    m_in_gc_time_ = false;

    if (m_last_immediately_destroyed_objects_ != 0)
    {
        Logger::Log<Object>("Immediately destroyed %d objects.", m_last_immediately_destroyed_objects_);
        m_last_immediately_destroyed_objects_ = 0;
    }

    if (!g_destroyed_objects.empty())
    {
        Logger::Log<Object>("Destroyed %d objects.", g_destroyed_objects.size());
    }
}

std::string Object::GenerateName()
{
    m_last_instantiated_name_count_++;
    return "Instantiated Object " + std::to_string(m_last_instantiated_name_count_);
}

xg::Guid Object::GenerateGuid()
{
    xg::Guid guid;
    do
    {
        guid = xg::newGuid();
    } while (m_objects_.contains(guid));

    return guid;
}

void Object::SetGuid(const xg::Guid new_guid)
{
    const auto pos = m_objects_.find(m_guid_);
    if (pos != m_objects_.end() && pos->second.get() == this)
    {
        m_objects_.erase(pos);
    }

    m_guid_ = new_guid;
    m_objects_.emplace(m_guid_, shared_from_this());
}

xg::Guid Object::Guid() const
{
    return m_guid_;
}

std::string Object::Name() const
{
    return m_name_;
}

void Object::SetName(const std::string &name)
{
    m_name_ = name;
}

bool Object::IsDestroying() const
{
    return m_is_destroying_;
}

void Object::DestroyThis()
{
    Destroy(shared_from_this());
}

void Object::Destroy(const std::shared_ptr<Object> &obj)
{
    obj->m_is_destroying_ = true;
    g_destroying_objects.emplace_back(obj);
}

void Object::DestroyImmediate(const std::shared_ptr<Object> &obj)
{
    if (obj == nullptr)
    {
        return;
    }

    if (m_in_gc_time_)
    {
        Logger::Warn<Object>("Cannot immediately destroy object `%s` during GC cycle. Use Object::Destroy instead.",
                             obj->Name().c_str());
        Destroy(obj);
        return;
    }

    if (UpdateManager::InUpdateCycle() || UpdateManager::InFixedUpdateCycle())
    {
        Logger::Warn<Object>(
        "Cannot immediately destroy object `%s` during UpdateManager cycle. Use Object::Destroy instead.",
        obj->Name().c_str());
        Destroy(obj);
        return;
    }

    if (obj->IsDestroying())
    {
        Logger::Warn<Object>("Object is already destroying.");
        return;
    }

    obj->m_is_destroying_ = true;
    obj->OnDestroy();
    m_objects_.erase(obj->m_guid_);
    ++m_last_immediately_destroyed_objects_;
}

std::shared_ptr<Object> Object::Find(const xg::Guid &guid)
{
    if (m_objects_.contains(guid))
        return m_objects_.at(guid);
    return nullptr;
}
}