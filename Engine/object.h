#pragma once
#include <memory>
#include <string>
#include "enable_shared_from_base.h"
#include<cereal/archives/binary.hpp>
#include<cereal/archives/portable_binary.hpp>
#include<cereal/archives/xml.hpp>
#include<cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

namespace engine
{
class Object : public enable_shared_from_base<Object>
{
public:
    virtual ~Object() = default;

    virtual void OnConstructed()
    {}

    std::string Name()
    {
        return m_name_;
    }

    virtual void SetName(const std::string &name)
    {
        m_name_ = name;
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

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(CEREAL_NVP(m_name_));
    }

protected:
    std::string m_name_ = "Unknown Object";

    Object() = default;

};
}