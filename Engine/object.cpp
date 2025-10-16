#include "pch.h"
#include "serializer.h"
#include "object_util.h"
#include "update_manager.h"

namespace engine
{

unsigned int Object::m_last_instantiated_name_count_ = 0;
unsigned int Object::m_last_immediately_destroyed_objects_ = 0;
std::unordered_map<xg::Guid, std::shared_ptr<Object>> Object::m_objects_;
std::vector<std::shared_ptr<Object>> Object::m_destroying_objects_;
std::vector<std::shared_ptr<Object>> Object::m_destroyed_objects_;

void Object::GarbageCollect()
{
    for (const auto &obj : m_destroying_objects_)
    {
        obj->OnDestroy();
        m_objects_.erase(obj->m_guid_);
    }

    if (m_last_immediately_destroyed_objects_ != 0)
    {
        Logger::Log<Object>("Immediately destroyed %d objects.", m_last_immediately_destroyed_objects_);
        m_last_immediately_destroyed_objects_ = 0;
    }

    m_destroyed_objects_.clear();
    m_destroyed_objects_.insert(m_destroyed_objects_.begin(),
                                m_destroying_objects_.begin(), m_destroying_objects_.end());

    if (!m_destroying_objects_.empty())
    {
        Logger::Log<Object>("Destroyed %d objects.", m_destroying_objects_.size());
        m_destroying_objects_.clear();
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
    m_destroying_objects_.emplace_back(obj);
}

void Object::DestroyImmediate(const std::shared_ptr<Object> &obj)
{
    if (UpdateManager::InUpdateCycle() || UpdateManager::InFixedUpdateCycle())
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
    m_objects_.erase(obj->m_guid_);
    ++m_last_immediately_destroyed_objects_;
}

std::shared_ptr<Object> Object::Find(const xg::Guid &guid)
{
    if (m_objects_.contains(guid))
        return m_objects_.at(guid);
    return nullptr;
}

std::shared_ptr<Object> Object::Instantiate(const std::shared_ptr<Object> &original)
{
    std::stringstream ss;
    {
        Serializer serializer;
        serializer.Save(ss, original);
    }

    const std::string serialized_object(ss.view());
    const auto serialized_clone_object = ObjectUtil::MakeClone(serialized_object);

    std::istringstream is(serialized_clone_object);
    Serializer deserializer;
    auto cloned_object = deserializer.Load<Object>(is);
    cloned_object->SetName(ObjectUtil::GetDeduplicatedName(cloned_object));
    cloned_object->OnConstructed();
    return cloned_object;
}
}