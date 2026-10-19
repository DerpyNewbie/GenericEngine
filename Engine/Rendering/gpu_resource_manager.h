#pragma once
#include "gpu_resource_group.h"
#include "material.h"
#include "CabotEngine/Graphics/ByteAddressBuffer.h"

namespace engine
{
/// <summary>
/// MaterialBlockごとのGpuResourceGroupと、名前で共有されるグローバルなBufferを管理するクラスです。
/// </summary>
class GpuResourceManager
{
    inline static std::map<std::shared_ptr<MaterialBlock>, std::shared_ptr<GpuResourceGroup>> m_material_block_buffer_map_;
    inline static std::unordered_map<std::string, std::shared_ptr<BufferBase>> m_global_resources_;

public:
    /// <summary>
    /// MaterialBlockに対応するGpuResourceGroupを取得します。存在しない場合はMaterialBlockの各データからBufferを生成して作成します。
    /// </summary>
    /// <param name="material_block">対象のMaterialBlock</param>
    static std::shared_ptr<GpuResourceGroup> GetBuffersForMaterial(std::shared_ptr<MaterialBlock> material_block);
    /// <summary>
    /// グローバルなBufferを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>登録されていない場合はnullptr</returns>
    static std::shared_ptr<BufferBase> GetGlobalBuffer(const std::string &name);

    /// <summary>
    /// BufferDataからグローバルなBufferを設定します。未登録の場合は種類に応じたBufferを生成し、登録済みの場合はデータを更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="buffer_data">設定するデータ</param>
    static void SetGlobalBufferData(const std::string &name, const std::shared_ptr<BufferDataBase> &buffer_data);
    /// <summary>
    /// ConstantBufferをグローバルなBufferとして登録します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="buffer">登録するBuffer</param>
    static void SetGlobalBuffer(const std::string &name, const std::shared_ptr<ConstantBuffer> &buffer);
    /// <summary>
    /// StructuredBufferをグローバルなBufferとして登録します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="buffer">登録するBuffer</param>
    static void SetGlobalBuffer(const std::string &name, const std::shared_ptr<StructuredBuffer> &buffer);
    /// <summary>
    /// ByteAddressBufferをグローバルなBufferとして登録します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="buffer">登録するBuffer</param>
    static void SetGlobalBuffer(const std::string &name, const std::shared_ptr<ByteAddressBuffer> &buffer);
    /// <summary>
    /// TextureをグローバルなTextureとして登録します。
    /// </summary>
    /// <param name="name">Shader上のTexture名</param>
    /// <param name="texture">登録するTexture</param>
    static void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture2D> &texture);
    /// <summary>
    /// intをグローバルな値として設定します。未登録の場合はConstantBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalInt(const std::string &name, int data);
    /// <summary>
    /// floatをグローバルな値として設定します。未登録の場合はConstantBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalFloat(const std::string &name, float data);
    /// <summary>
    /// floatの配列をグローバルな値として設定します。未登録の場合はStructuredBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalFloatArray(const std::string &name, const std::vector<float> &data);
    /// <summary>
    /// Vector3をグローバルな値として設定します。未登録の場合はConstantBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalVector(const std::string &name, const Vector3 &data);
    /// <summary>
    /// Vector3の配列をグローバルな値として設定します。未登録の場合はStructuredBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalVectorArray(const std::string &name, const std::vector<Vector3> &data);
    /// <summary>
    /// Matrixをグローバルな値として設定します。未登録の場合はConstantBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalMatrix(const std::string &name, const Matrix &data);
    /// <summary>
    /// Matrixの配列をグローバルな値として設定します。未登録の場合はStructuredBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalMatrixArray(const std::string &name, const std::vector<Matrix> &data);
    /// <summary>
    /// Colorをグローバルな値として設定します。未登録の場合はConstantBufferを生成し、登録済みの場合はBufferの内容を更新します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="data">設定する値</param>
    static void SetGlobalColor(const std::string &name, const Color &data);
};
}