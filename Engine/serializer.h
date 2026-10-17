#pragma once

namespace engine
{
/// <summary>
/// Objectの保存と読み込み(Serialize / Deserialize)を行うクラスです。
/// </summary>
class Serializer
{
public:
    /// <summary>
    /// ObjectをJSONとしてstreamに書き出します。
    /// </summary>
    /// <param name="output_stream">書き出し先</param>
    /// <param name="save_resource">保存するObject</param>
    /// <param name="pretty">インデントを付けて出力する場合 true</param>
    /// <returns>成功した場合 true</returns>
    template <typename T>
    [[nodiscard]] bool Save(std::ostream &output_stream, std::shared_ptr<T> save_resource, const bool pretty = true)
    {
        static_assert(std::is_base_of<Object, T>(), "Base type is not Object.");
        if (save_resource == nullptr)
        {
            Logger::Error<Serializer>("Failed to save resource: nullptr");
            return false;
        }

        try
        {
            std::stringstream ss;

            {
                const cereal::JSONOutputArchive::Options options = pretty ? cereal::JSONOutputArchive::Options::Default() : cereal::JSONOutputArchive::Options::NoIndent();
                cereal::JSONOutputArchive o_archive(ss, options);
                o_archive(save_resource);
            }

            const std::string buff = ss.str();
            output_stream.write(buff.c_str(), static_cast<std::streamsize>(buff.size()));
            return true;
        }
        catch (const std::exception &e)
        {
            Logger::Error<Serializer>("Failed to save resource '%s': %s", save_resource->Name().c_str(), e.what());
            return false;
        }
    }

    /// <summary>
    /// streamのJSONから T のObjectを読み込みます。
    /// </summary>
    /// <param name="input_stream">読み込み元</param>
    /// <returns>読み込まれたObject。失敗した場合 nullptr</returns>
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