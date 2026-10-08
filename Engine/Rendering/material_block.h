#pragma once
#include "material_data.h"
#include "shader.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
/// <summary>
/// MaterialDataと、そのDescriptorHandleの組です。
/// </summary>
struct MaterialDataPair
{
    std::shared_ptr<IMaterialData> data = nullptr;
    std::shared_ptr<DescriptorHandle> handle = nullptr;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(data));
    }
};

/// <summary>
/// CBV / SRV / UAVそれぞれのデータの数を持つ構造体です。
/// </summary>
struct ShaderDataIndex
{
    int cbv_length = 0;
    int srv_length = 0;
    int uav_length = 0;

    /// <summary>
    /// 指定された種類のデータの数を持つメンバーへのポインタを取得します。
    /// </summary>
    int *GetLengthField(kParameterBufferType type);
    /// <summary>
    /// 指定された種類のデータの数を取得します。
    /// </summary>
    int GetLength(kParameterBufferType type) const;
    /// <summary>
    /// material_dataの中で、指定された種類のデータが始まる位置を取得します。CBV、SRV、UAVの順に並びます。
    /// </summary>
    int GetOffset(kParameterBufferType type) const;
    /// <summary>
    /// すべての種類のデータの数の合計を取得します。
    /// </summary>
    int GetFullLength() const;

    template <typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(cbv_length), CEREAL_NVP(srv_length), CEREAL_NVP(uav_length));
    }
};

/// <summary>
/// Shared shader parameters.
/// </summary>
/// <remarks>
/// Used by Material for better memory-management among the same objects
/// </remarks>
class MaterialBlock : public Object, public Inspectable
{
public:
    ShaderDataIndex shader_index = {};

    std::vector<MaterialDataPair> material_data = {};

    MaterialBlock() = default;
    /// <summary>
    /// 確保しているDescriptorHandleをすべて解放します。
    /// </summary>
    ~MaterialBlock() override;

    void OnInspectorGui() override;

    /// <summary>
    /// 指定された名前のparameterに値を設定します。
    /// </summary>
    /// <param name="name">parameterの名前</param>
    /// <param name="material_data">設定する値</param>
    /// <returns>parameterが見つからない、または型が違う場合 false</returns>
    template <typename T>
    bool SetMaterialData(const std::string &name, T material_data);

    /// <summary>
    /// Shaderのparameterごとにデータを作成して追加します。resource_material_dataに同じ名前のデータがある場合は、それを使います。
    /// </summary>
    /// <param name="shader_params">Shaderのparameterのリスト</param>
    /// <param name="resource_material_data">引き継ぐデータ</param>
    void LoadShaderParameters(
        const std::vector<ShaderParameter> &shader_params,
        const std::vector<MaterialDataPair> &resource_material_data = {}
    );

    /// <summary>
    /// データを、種類ごとの並びを保つ位置に追加します。
    /// </summary>
    /// <param name="data">追加するデータ</param>
    void Insert(const std::shared_ptr<IMaterialData> &data);
    /// <summary>
    /// 指定された種類のデータが1つもないかどうかを取得します。
    /// </summary>
    bool Empty(kParameterBufferType buffer_type);
    /// <summary>
    /// 指定された種類のデータの先頭のIteratorを取得します。
    /// </summary>
    std::vector<MaterialDataPair>::iterator Begin(kParameterBufferType buffer_type);
    /// <summary>
    /// 指定された種類のデータの末尾の次のIteratorを取得します。
    /// </summary>
    std::vector<MaterialDataPair>::iterator End(kParameterBufferType buffer_type);

    /// <summary>
    /// 名前からデータを取得します。
    /// </summary>
    /// <param name="name">parameterの名前</param>
    /// <returns>見つからない場合 nullptr</returns>
    std::shared_ptr<IMaterialData> FindMaterialDataByName(const std::string &name);

    /// <summary>
    /// 変更されたデータのBufferを更新し、DescriptorHandleを持っていないデータにはDescriptorHandleを割り当てます。
    /// </summary>
    void UpdateBuffer();
    /// <summary>
    /// まだBufferに反映されていない変更があるかどうかを取得します。
    /// </summary>
    bool IsDirty();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(material_data)
        );

        if (version >= 2)
        {
            ar(
                CEREAL_NVP(shader_index)
            );
        }
    }
};

template <typename T>
bool MaterialBlock::SetMaterialData(const std::string &name, T material_data)
{
    for (auto &data : this->material_data | std::views::transform(&MaterialDataPair::data))
    {
        if (data->parameter.name == name)
        {
            auto casted_data = std::dynamic_pointer_cast<MaterialData<T>>(data);
            if (casted_data == nullptr)
                return false;
            casted_data->SetValue(material_data);
            return true;
        }
    }
    return false;
}

}

CEREAL_CLASS_VERSION(engine::MaterialBlock, 2)