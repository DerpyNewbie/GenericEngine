#pragma once

namespace engine
{
/// <summary>
/// Objectの名前の重複回避や複製など、Objectに関する便利関数をまとめたクラスです。
/// </summary>
class ObjectUtil
{
public:
    /// <summary>
    /// 兄弟のGameObjectと名前が重複しないよう、"名前 (N)"形式の名前を取得します。GameObject以外の場合は元の名前をそのまま返します。
    /// </summary>
    /// <param name="object">対象のObject</param>
    static std::string GetDeduplicatedName(const std::shared_ptr<Object> &object);
    /// <summary>
    /// "名前 (N)"形式の名前を、元の名前と番号に分解します。
    /// </summary>
    /// <param name="name">分解する名前</param>
    /// <returns>元の名前と番号。番号がない場合は 0</returns>
    static std::pair<std::string, int> GetOriginalName(const std::string &name);
    /// <summary>
    /// SerializeされたObjectのJSONに含まれるGuidを、すべて新しいGuidに置き換えたJSONを作成します。AssetPtrの参照先も、複製対象に含まれるものは新しいGuidに置き換えます。
    /// </summary>
    /// <param name="object_json">SerializeされたObjectのJSON</param>
    static std::string MakeClone(const std::string &object_json);
    /// <summary>
    /// JSONのObjectが、engine::ObjectをSerializeしたもの(m_guid_とm_name_のみを持つ)かどうかを判定します。
    /// </summary>
    static bool IsEngineObject(const rapidjson::Document::Object &object);
    /// <summary>
    /// JSONのObjectが、AssetPtrをSerializeしたものかどうかを判定します。
    /// </summary>
    static bool IsAssetPtr(const rapidjson::Document::Object &object);
    /// <summary>
    /// JSONのObjectを子まですべて探索し、条件に合うObjectをすべて取得します。
    /// </summary>
    /// <param name="object">探索を始めるObject</param>
    /// <param name="pred">条件を判定する関数</param>
    static std::vector<rapidjson::Document::Object> FindMatchingObjects(const rapidjson::Document::Object &object,
                                                                        const std::function<bool(
                                                                        rapidjson::Document::Object)> &pred);

};
}