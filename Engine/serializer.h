#pragma once
#include "../pch.h"

#include "object.h"
#include "cereal/archives/json.hpp"

namespace engine
{
class Serializer
{
public:
    template <typename T>
    void Save(std::shared_ptr<T> save_resource)
    {
        static_assert(std::is_base_of<Object, T>(),
                      "Base type is not Object.");
        std::string name = save_resource->Name() + ".json";
        std::ofstream os(name);
        cereal::JSONOutputArchive o_archive(os);

        o_archive(save_resource);
    }

    template <typename T>
    std::shared_ptr<T> Load(const std::string &file_name)
    {
        static_assert(std::is_base_of<Object, T>(),
                      "Base type is not Object.");

        auto load_resource = std::make_shared<T>();
        std::ifstream is(file_name);
        cereal::JSONInputArchive i_archive(is);

        i_archive(load_resource);
        return load_resource;
    }
};
}