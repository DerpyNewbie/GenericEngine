#pragma once
#include "asset_hierarchy.h"
#include "asset_descriptor.h"
#include "object.h"

namespace engine
{
using namespace std::filesystem;

class AssetDatabase
{
    friend class Engine;

    static path m_project_directory_;
    static std::shared_ptr<AssetHierarchy> m_asset_hierarchy_;
    static std::unordered_map<std::string, std::vector<std::shared_ptr<AssetDescriptor>>> m_assets_map_by_type_;

    static void Init();

public:
    /// <summary>
    /// Sets the project directory to load assets from.
    /// </summary>
    /// <remarks>
    /// Calling this will unload all assets that are currently active.
    /// </remarks>
    /// <param name="path">Path to the project asset directory.</param>
    static void SetProjectDirectory(const path &path);

    /// <summary>
    /// Imports all assets from the specified path.
    /// </summary>
    /// <param name="path">Path to the asset path</param>
    static void Import(const path &path);

    /// <summary>
    /// Import all assets from the project directory.
    /// </summary>
    static void ImportAll();

    /// <summary>
    /// Retrieves a path of the project directory.
    /// </summary>
    /// <returns>Absolute path of the current project directory.</returns>
    static path GetProjectDirectory();

    /// <summary>
    /// Get or create AssetHierarchy at a corresponding path.
    /// </summary>
    /// <param name="path">A path to retrieve AssetHierarchy from.</param>
    /// <returns>AssetHierarchy instance for the corresponding path.</returns>
    static std::shared_ptr<AssetHierarchy> GetOrCreateAssetHierarchy(const path &path);

    /// <summary>
    /// Retrieves the root of AssetHierarchy.
    /// </summary>
    /// <returns>The root AssetHierarchy</returns>
    static std::shared_ptr<AssetHierarchy> GetRootAssetHierarchy();

    /// <summary>
    /// Retrieves the AssetDescriptor at a corresponding path.
    /// </summary>
    /// <param name="path_file">A path to retrieve AssetDescriptor from.</param>
    /// <returns>Corresponding AssetDescriptor if the path points to a file, `nullptr` otherwise. </returns>
    static std::shared_ptr<AssetDescriptor> GetAssetDescriptor(const path &path_file);

    /// <summary>
    /// Retrieves all child AssetDescriptors from the directory.
    /// </summary>
    /// <param name="path_directory">A path to retrieve all child AssetDescriptors from.</param>
    /// <returns>List of AssetDescriptors</returns>
    static std::vector<std::shared_ptr<AssetDescriptor>> GetAssetDescriptors(const path &path_directory);

    /// <summary>
    /// Retrieves all AssetDescriptors with a specified type
    /// </summary>
    /// <param name="file_extension">A file extension that begins with the dot '.'. E.x. ".txt"</param>
    /// <returns>List of AssetDescriptors that has extension of <paramref name="file_extension"/></returns>
    static std::vector<std::shared_ptr<AssetDescriptor>> GetAssetDescriptorsByType(const std::string &file_extension);

    /// <summary>
    /// Retrieve the imported object from a corresponding path. 
    /// </summary>
    /// <param name="path">A path to retrieve AssetDescriptor from.</param>
    /// <returns>The imported object if import has been successful. `nullptr` otherwise.</returns>
    static std::shared_ptr<Object> GetAsset(const path &path);

    /// <summary>
    /// Retrieve all imported objects with a specified type 
    /// </summary>
    /// <param name="file_extension">A file extension that begins with the dot '.'. E.x. ".txt"</param>
    /// <returns>List of the imported objects that has extension of <paramref name="file_extension"/></returns>
    static std::vector<std::shared_ptr<Object>> GetAssetsByType(const std::string &file_extension);

    /// <summary>
    /// Saves the changed AssetDescriptor.
    /// </summary>
    /// <param name="asset_descriptor">An AssetDescriptor to write</param>
    static void SaveAsset(std::shared_ptr<AssetDescriptor> asset_descriptor);
};
}