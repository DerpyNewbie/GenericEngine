#pragma once
#include "asset_descriptor.h"

namespace engine
{
/// <summary>
/// Assetのファイル / ディレクトリの階層を表すObjectです。
/// </summary>
class AssetHierarchy : public Object
{
    friend class AssetDatabase;

    bool m_is_file_;
    bool m_is_directory_;

public:
    std::shared_ptr<AssetDescriptor> asset; // can be null
    std::weak_ptr<AssetHierarchy> parent; // can be null at root
    std::vector<std::shared_ptr<AssetHierarchy>> children; // can be empty

    /// <summary>
    /// ファイルを表しているかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsFile() const;

    /// <summary>
    /// Directoryを表しているかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsDirectory() const;

    /// <summary>
    /// 親のchildrenから自身を取り除き、子をすべて破棄します。
    /// </summary>
    void OnDestroy() override;
};
}