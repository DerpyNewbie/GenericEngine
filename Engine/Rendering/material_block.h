#pragma once
#include "shader.h"
#include "MaterialData/byte_address_buffer_data.h"
#include "MaterialData/constant_buffer_data.h"
#include "MaterialData/structured_buffer_data.h"
#include "MaterialData/texture_buffer_data.h"
#include "MaterialData/texture_cube_buffer_data.h"
#include "MaterialData/uav_texture_buffer_data.h"

namespace engine
{
/// <summary>
/// Materialが持つ、Shaderの各パラメーターに渡すBufferDataを名前ごとにまとめたクラスです。
/// </summary>
class MaterialBlock : public Object, public Inspectable
{

    std::unordered_map<std::string, std::shared_ptr<BufferDataBase>> m_buffer_data_;

public:
    MaterialBlock() = default;

    void OnInspectorGui() override;

    /// <summary>
    /// 指定した名前のConstantBufferDataを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>見つからない、または種類が異なる場合はnullptr</returns>
    std::shared_ptr<ConstantBufferData> GetConstantBufferData(const std::string &name);
    /// <summary>
    /// 指定した名前のStructuredBufferDataを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>見つからない、または種類が異なる場合はnullptr</returns>
    std::shared_ptr<StructuredBufferData> GetStructuredBufferData(const std::string &name);
    /// <summary>
    /// 指定した名前のTextureBufferDataを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>見つからない、または種類が異なる場合はnullptr</returns>
    std::shared_ptr<TextureBufferData> GetTextureBufferData(const std::string &name);
    /// <summary>
    /// 指定した名前のUavTextureBufferDataを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>見つからない、または種類が異なる場合はnullptr</returns>
    std::shared_ptr<UavTextureBufferData> GetUavTextureBufferData(const std::string &name);
    /// <summary>
    /// 指定した名前のByteAddressBufferDataを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>見つからない場合はnullptr</returns>
    std::shared_ptr<ByteAddressBufferData> GetByteAddressBufferData(const std::string &name);
    /// <summary>
    /// 指定した名前のTextureCubeBufferDataを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>見つからない、または種類が異なる場合はnullptr</returns>
    std::shared_ptr<TextureCubeBufferData> GetTextureCubeData(const std::string &name);

    /// <summary>
    /// Shaderのパラメーターごとに、種類に応じたBufferDataを生成して追加します。既に同じ名前のデータがある場合は追加しません。
    /// </summary>
    /// <param name="shader_params">Shaderのパラメーター</param>
    void LoadShaderParameters(const std::vector<ShaderParameter> &shader_params);

    /// <summary>
    /// 指定した名前の変数を持つConstantBufferDataを探します。
    /// </summary>
    /// <param name="variable_name">変数名（例: "_Color"）</param>
    /// <returns>見つからない場合はnullptr</returns>
    std::shared_ptr<ConstantBufferData> FindConstantBufferData(const std::string &variable_name);

    /// <summary>
    /// すべてのBufferDataを、(Shader上の名前, BufferData)として順に渡します。
    /// </summary>
    /// <param name="action">BufferDataごとに呼ぶ関数</param>
    void ForEachBufferData(const std::function<void(const std::string &, BufferDataBase &)> &action) const;
    /// <summary>
    /// いずれかのBufferDataが変更されていればtrueを返し、変更済みの印を消します。
    /// </summary>
    bool ConsumeDirty();
    
    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(m_buffer_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::MaterialBlock, 2)