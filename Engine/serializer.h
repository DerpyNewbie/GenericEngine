#pragma once
#include <fstream>
#include "object.h"
#include "cereal/archives/json.hpp"
#include <cereal/types/polymorphic.hpp>

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
        cereal::JSONOutputArchive oArchive(os);
        
        oArchive(save_resource);
    }
    
    template <typename T>
    void Load(std::shared_ptr<T> load_resource)
    {
        static_assert(std::is_base_of<Object, T>(),
              "Base type is not Object.");

        std::string name = load_resource->Name() + ".json";
        std::ifstream is(name);
        cereal::JSONInputArchive iArchive(is);
        
        iArchive(load_resource);
    }
};
}