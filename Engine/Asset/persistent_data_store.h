#pragma once

namespace engine
{
/// <summary>
/// metaファイルなどのjsonに、Key-Valueの形で値を読み書きするためのクラスです。
/// </summary>
class PersistentDataStore
{
    rapidjson::Document *m_document_;
    rapidjson::Value *m_value_;

    /// <summary>
    /// keyに対応するJSONのメンバーのIteratorを取得します。
    /// </summary>
    [[nodiscard]] auto Find(const std::string &key) const;
    /// <summary>
    /// keyに値を設定します。keyが存在しない場合は追加します。
    /// </summary>
    void Set(const std::string &key, rapidjson::Value &value) const;
    /// <summary>
    /// JSONのメモリ確保に使うAllocatorを取得します。
    /// </summary>
    [[nodiscard]] rapidjson::MemoryPoolAllocator<> &Allocator() const;

public:
    /// <summary>
    /// 指定されたJSONのObjectを読み書きするDataStoreを作成します。
    /// </summary>
    /// <param name="document">メモリの確保に使うDocument</param>
    /// <param name="value">読み書きの対象となるJSONのObject</param>
    explicit PersistentDataStore(rapidjson::Document *document, rapidjson::Value *value) :
        m_document_(document), m_value_(value)
    { }

    /// <summary>
    /// keyが存在するかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool HasKey(const std::string &key) const;
    /// <summary>
    /// keyを削除します。
    /// </summary>
    void RemoveKey(const std::string &key) const;
    /// <summary>
    /// すべてのkeyを取得します。
    /// </summary>
    [[nodiscard]] std::vector<std::string> GetKeys() const;
    /// <summary>
    /// すべてのkeyを削除します。
    /// </summary>
    void ClearKeys() const;

    /// <summary>
    /// keyに文字列を設定します。
    /// </summary>
    void SetString(const std::string &key, const std::string &value) const;
    /// <summary>
    /// keyにintを設定します。
    /// </summary>
    void SetInt(const std::string &key, int value) const;
    /// <summary>
    /// keyにfloatを設定します。
    /// </summary>
    void SetFloat(const std::string &key, float value) const;
    /// <summary>
    /// keyにboolを設定します。
    /// </summary>
    void SetBool(const std::string &key, bool value) const;
    /// <summary>
    /// keyに別のDataStoreの内容をコピーして設定します。
    /// </summary>
    void SetDataStore(const std::string &key, const PersistentDataStore &value) const;
    /// <summary>
    /// keyにJSONの値を直接設定します。
    /// </summary>
    void SetValue(const std::string &key, rapidjson::Value &value) const;

    /// <summary>
    /// keyの文字列を取得します。
    /// </summary>
    /// <param name="default_value">keyが存在しない、または文字列でない場合に返す値</param>
    [[nodiscard]] std::string GetString(const std::string &key, const std::string &default_value = "") const;
    /// <summary>
    /// keyのintを取得します。
    /// </summary>
    /// <param name="default_value">keyが存在しない、または数値でない場合に返す値</param>
    [[nodiscard]] int GetInt(const std::string &key, const int &default_value = 0) const;
    /// <summary>
    /// keyのfloatを取得します。
    /// </summary>
    /// <param name="default_value">keyが存在しない、または数値でない場合に返す値</param>
    [[nodiscard]] float GetFloat(const std::string &key, const float &default_value = 0.0f) const;
    /// <summary>
    /// keyのboolを取得します。
    /// </summary>
    /// <param name="default_value">keyが存在しない、またはboolでない場合に返す値</param>
    [[nodiscard]] bool GetBool(const std::string &key, const bool &default_value = false) const;
    /// <summary>
    /// keyのObjectを読み書きするDataStoreを取得します。keyが存在しない場合は空のObjectを作成します。
    /// </summary>
    [[nodiscard]] PersistentDataStore GetDataStore(const std::string &key) const;
    /// <summary>
    /// keyのJSONの値を直接取得します。keyが存在している必要があります。
    /// </summary>
    [[nodiscard]] rapidjson::Value &GetValue(const std::string &key) const;

    /// <summary>
    /// keyの値が文字列であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsString(const std::string &key) const;
    /// <summary>
    /// keyの値が数値であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsNumber(const std::string &key) const;
    /// <summary>
    /// keyの値がboolであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsBool(const std::string &key) const;
    /// <summary>
    /// keyの値がObjectであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsDataStore(const std::string &key) const;

    /// <summary>
    /// DataStoreの内容をrapidjsonのWriterに書き出します。
    /// </summary>
    template <typename Writer>
    void Write(Writer &writer) const
    {
        m_value_->Accept(writer);
    }
};
}