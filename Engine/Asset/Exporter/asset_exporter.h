#pragma once

namespace engine
{
struct AssetDescriptor;
}

namespace engine
{
class AssetExporter
{
    static std::unordered_map<std::string, std::shared_ptr<AssetExporter>> m_asset_exporters_;

public:
    virtual ~AssetExporter() = default;

    /// <summary>
    /// List of supported file extensions for the AssetImporter implementation.
    /// </summary>
    /// <returns>A file extension that begins with the dot '.'. E.x. ".txt"</returns>
    virtual std::vector<std::string> SupportedExtensions() = 0;

    /// <summary>
    /// Write Object to the file that AssetDescriptor points to.
    /// </summary>
    /// <param name="descriptor">A valid AssetDescriptor</param>
    virtual void Export(AssetDescriptor *descriptor) = 0;

    /// <summary>
    /// Add AssetExporter implementations to make impl retrievable with supported file extension.
    /// </summary>
    /// <param name="exporter">AssetExporter instance</param>
    static void Register(const std::shared_ptr<AssetExporter> &exporter);

    /// <summary>
    /// Add AssetExporter implementations to make impl retrievable with specified file extension.
    /// </summary>
    /// <param name="file_extension">A file extension that begins with the dot '.'. E.x. ".txt"</param>
    /// <param name="exporter">AssetExporter instance</param>
    static void Register(const std::string &file_extension, const std::shared_ptr<AssetExporter> &exporter);

    /// <summary>
    /// Retrieve AssetExporter implementation that supports file extension.
    /// </summary>
    /// <param name="file_extension">A file extension that begins with the dot '.'. E.x. ".txt"</param>
    /// <returns>AssetExporter instance if a supported instance were found. `nullptr` otherwise.</returns>
    static std::shared_ptr<AssetExporter> Get(const std::string &file_extension);
};
}