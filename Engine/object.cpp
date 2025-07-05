#include "pch.h"

#include "object.h"

#include "logger.h"
#include "update_manager.h"

namespace engine
{
unsigned int Object::m_last_immediately_destroyed_objects_ = 0;
std::vector<std::shared_ptr<Object>> Object::m_destroying_objects_;

void Object::DestroyObjects()
{
    for (const auto &obj : m_destroying_objects_)
    {
        obj->OnDestroy();
    }

    if (m_last_immediately_destroyed_objects_ != 0)
    {
        Logger::Log<Object>("Immediately destroyed %d objects.", m_last_immediately_destroyed_objects_);
        m_last_immediately_destroyed_objects_ = 0;
    }

    if (!m_destroying_objects_.empty())
    {
        Logger::Log<Object>("Destroyed %d objects.", m_destroying_objects_.size());
        m_destroying_objects_.clear();
    }
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
    m_destroying_objects_.emplace_back(obj);
}

void Object::DestroyImmediate(const std::shared_ptr<Object> &obj)
{
    if (UpdateManager::InUpdateCycle() || UpdateManager::InFixedUpdateCycle() || UpdateManager::InDrawCallCycle())
    {
        Logger::Warn<Object>(
            "Cannot immediately destroy object during UpdateManager cycle. Use Object::Destroy instead.");
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
    ++m_last_immediately_destroyed_objects_;
}
}