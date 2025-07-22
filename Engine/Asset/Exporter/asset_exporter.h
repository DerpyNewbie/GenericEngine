#pragma once

namespace engine
{
struct AssetDescriptor;
}

namespace engine
{
class AssetExporter
{
    static std::set<std::shared_ptr<AssetExporter>> m_exporters_;
    static std::unordered_map<std::string, std::shared_ptr<AssetExporter>> m_exporter_extension_map_;

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
    /// <param name="output_stream">An output stream to write Object</param>
    /// <param name="descriptor">A valid AssetDescriptor</param>
    virtual void Export(std::ostream &output_stream, AssetDescriptor *descriptor) = 0;

    /// <summary>
    /// Check if Object can be exported with this exporter.
    /// </summary>
    /// <param name="object">Object to check the ability to export</param>
    /// <returns>true if the exporter supports an object, false otherwise.</returns>
    virtual bool CanExport(const std::shared_ptr<Object> &object) = 0;

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

    /// <summary>
    /// Retrieve AssetExporter implementation that supports derived Object.
    /// </summary>
    /// <param name="object">An object to check against exporter.</param>
    /// <returns>AssetExporter instance that supports exporting a provided object. `nullptr` otherwise.</returns>
    static std::shared_ptr<AssetExporter> Get(const std::shared_ptr<Object> &object);

    /// <summary>
    /// Retrieve all registered AssetExporter instances.
    /// </summary>
    /// <returns>Set of registered AssetExporter instances.</returns>
    static std::set<std::shared_ptr<AssetExporter>> Get();
};
}