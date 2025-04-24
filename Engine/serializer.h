#pragma once
#include <fstream>
#include "object.h"
#include "cereal/archives/json.hpp"
#include <cereal/types/polymorphic.hpp>

class Serializer
{
public:
    template <typename T>
    void Save(T* save_resource)
    {
        auto resource = dynamic_cast<engine::Object*>(save_resource);
        if (resource == nullptr) return;

        std::string name = resource->GetName() + ".json";
        std::ofstream os(name);
        cereal::JSONOutputArchive oArchive(os);
        
        oArchive(save_resource);
    }
    
    template <typename T>
    void Load(T* load_resource)
    {
        auto resource = dynamic_cast<engine::Object*>(load_resource);
        if (resource == nullptr) return;

        std::string name = resource->GetName() + ".json";
        std::ifstream is(name);
        cereal::JSONInputArchive iArchive(is);
        
        iArchive(load_resource);
    }
};
