#pragma once
#include "Asset/asset_descriptor.h"

namespace engine
{
class AssetImporter
{
    static std::set<std::shared_ptr<AssetImporter>> m_importers_;
    static std::unordered_map<std::string, std::shared_ptr<AssetImporter>> m_importer_extension_map_;

public:
    virtual ~AssetImporter() = default;

    /// <summary>
    /// List of supported file extensions for the AssetImporter implementation.
    /// </summary>
    /// <returns>A file extension that begins with the dot '.'. E.x. ".txt"</returns>
    virtual std::vector<std::string> SupportedExtensions() = 0;

    /// <summary>
    /// Check if Object can be exported with this exporter.
    /// </summary>
    /// <param name="object">Object to check the ability to export</param>
    /// <returns>true if the exporter supports an object, false otherwise.</returns>
    virtual bool IsCompatibleWith(std::shared_ptr<Object> object)
    {
        return false;
    }

    /// <summary>
    /// Called when importing an object
    /// </summary>
    virtual void OnImport(AssetDescriptor *ctx)
    {}

    /// <summary>
    /// Called when writing an object
    /// </summary>
    virtual void OnExport(AssetDescriptor *ctx)
    {}

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

    /// <summary>
    /// Retrieve AssetImporter implementation that supports provided object.
    /// </summary>
    /// <returns>AssetImporter instance if a supported instance were found. `nullptr` otherwise.</returns>
    static std::shared_ptr<AssetImporter> Get(const std::shared_ptr<Object> &object);

    /// <summary>
    /// Retrieve all registered AssetImporter instances.
    /// </summary>
    /// <returns>Set of registered AssetImporter instances.</returns>
    static std::set<std::shared_ptr<AssetImporter>> Get();
};
}