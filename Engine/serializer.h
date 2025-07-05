#pragma once
#include "logger.h"
#include "../pch.h"

#include "object.h"
#include "cereal/archives/json.hpp"

namespace engine
{
class Serializer
{
public:
    template <typename T>
    void Save(std::ostream &output_stream, std::shared_ptr<T> save_resource)
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");
        cereal::JSONOutputArchive o_archive(output_stream);
        o_archive(save_resource);
    }

    template <typename T>
    std::shared_ptr<T> Load(std::istream &input_stream)
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");

        try
        {
            auto load_resource = std::make_shared<T>();
            cereal::JSONInputArchive i_archive(input_stream);

            i_archive(load_resource);
            return load_resource;
        }
        catch (std::exception)
        {
            Logger::Error("Failed to load resource.");
            return nullptr;
        }
    }
};
}