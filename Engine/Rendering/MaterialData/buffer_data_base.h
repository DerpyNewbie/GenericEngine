#pragma once
#include "Rendering/shader_parameter.h"

namespace engine
{
/// <summary>
/// Materialが持つ、Shaderの各Bufferに渡すCPU側データの基底クラスです。
/// </summary>
struct BufferDataBase : Inspectable
{
    bool is_dirty = true;
    ShaderParameter parameter;
    int upload_type = 0;

    BufferDataBase() = default;
    /// <summary>
    /// Shaderのパラメーター情報からBufferDataを生成します。
    /// </summary>
    /// <param name="param">対応するShaderのパラメーター</param>
    explicit BufferDataBase(const ShaderParameter &param);

    /// <summary>
    /// このデータが対応するShader上のBufferの種類を取得します。
    /// </summary>
    virtual kBufferType BufferType() = 0;

    template <typename Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(parameter),
            CEREAL_NVP(upload_type)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::BufferDataBase, 1)