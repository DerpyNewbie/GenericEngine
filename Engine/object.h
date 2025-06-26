#pragma once
#include <memory>
#include <string>

#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

#include "DxLib/dxlib_serializer.h"
#include "Math/math_serializer.h"

#include "enable_shared_from_base.h"

namespace engine
{
class Object : public enable_shared_from_base<Object>
{
    bool m_is_destroying_ = false;
    std::string m_name_ = "Unknown Object";

protected:
    Object() = default;

public:
    virtual ~Object() = default;

    virtual void OnConstructed()
    {}

    virtual void OnDestroy()
    {}

    std::string Name() const
    {
        return m_name_;
    }

    void SetName(const std::string &name)
    {
        m_name_ = name;
    }

    bool IsDestroying() const
    {
        return m_is_destroying_;
    }

    void DestroyThis()
    {
        Destroy(shared_from_this());
    }

    template <class T>
    static std::shared_ptr<T> Instantiate()
    {
        static_assert(std::is_base_of<Object, T>(),
                      "Base type is not Object.");
        auto obj = std::make_shared<T>();
        std::dynamic_pointer_cast<Object>(obj)->OnConstructed();
        return obj;
    }

    template <class T>
    static std::shared_ptr<T> Instantiate(const std::string &name)
    {
        static_assert(std::is_base_of<Object, T>(),
                      "Base type is not Object.");
        auto obj = std::make_shared<T>();
        auto ptr = std::dynamic_pointer_cast<Object>(obj);
        ptr->SetName(name);
        ptr->OnConstructed();
        return obj;
    }

    static void Destroy(const std::shared_ptr<Object> &obj)
    {
        obj->m_is_destroying_ = true;
    }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(m_name_));
    }
};
}