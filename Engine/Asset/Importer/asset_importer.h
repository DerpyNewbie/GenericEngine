#pragma once
#include "Asset/asset_descriptor.h"
#include "object.h"

namespace engine
{
class AssetImporter
{
    static std::unordered_map<std::string, std::shared_ptr<AssetImporter>> m_asset_importers_;

public:
    virtual ~AssetImporter() = default;

    /// <summary>
    /// List of supported file extensions for the AssetImporter implementation.
    /// </summary>
    /// <returns>A file extension that begins with the dot '.'. E.x. ".txt"</returns>
    virtual std::vector<std::string> SupportedExtensions() = 0;

    /// <summary>
    /// Retrieve Object from AssetDescriptor
    /// </summary>
    /// <param name="asset">A valid AssetDescriptor</param>
    /// <returns>Object representation of a file that AssetDescriptor points to. `nullptr` if the file cannot be converted to Object.</returns>
    virtual std::shared_ptr<Object> Import(AssetDescriptor *asset) = 0;

    /// <summary>
    /// Add AssetImporter implementations to make impl retrievable with supported file extension.
    /// </summary>
    /// <param name="importer">AssetImporter instance</param>
    static void Register(const std::shared_ptr<AssetImporter> &importer);

    /// <summary>
    /// Add AssetImporter implementations to make impl retrievable with specified file extension.
    /// </summary>
    /// <param name="file_extension">A file extension that begins with the dot '.'. E.x. ".txt"</param>
    /// <param name="importer">AssetImporter instance</param>
    static void Register(const std::string &file_extension, const std::shared_ptr<AssetImporter> &importer);

    /// <summary>
    /// Retrieve AssetImporter implementation that supports file extension.
    /// </summary>
    /// <param name="file_extension">A file extension that begins with the dot '.'. E.x. ".txt"</param>
    /// <returns>AssetImporter instance if a supported instance were found. `nullptr` otherwise.</returns>
    static std::shared_ptr<AssetImporter> Get(const std::string &file_extension);
};
}