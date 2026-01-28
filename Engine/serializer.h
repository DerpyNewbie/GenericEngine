#pragma once

namespace engine
{
class Serializer
{
public:
    template <typename T>
    [[nodiscard]] bool Save(std::ostream &output_stream, std::shared_ptr<T> save_resource, const bool pretty = true)
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");

        try
        {
            std::stringstream ss;

            {
                const cereal::JSONOutputArchive::Options options = pretty ? cereal::JSONOutputArchive::Options::Default() : cereal::JSONOutputArchive::Options::NoIndent();
                cereal::JSONOutputArchive o_archive(ss, options);
                o_archive(save_resource);
            }

            output_stream << ss.view();
            return true;
        }
        catch (const std::exception &e)
        {
            Logger::Error<Serializer>("Failed to save resource: %s", e.what());
            return false;
        }
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
        catch (const std::exception &e)
        {
            Logger::Error("Failed to load resource: %s", e.what());
            return nullptr;
        }
    }
};
}