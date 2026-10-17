#pragma once
#include "buffer_base.h"
#include "material_block.h"
#include "Asset/asset_ptr.h"

namespace engine
{
/// <summary>
/// GPUに渡す1つのリソースの情報です。Shader上の名前、Buffer、DescriptorHandle、Bufferの種類を持ちます。
/// </summary>
struct GpuResource
{
    std::string name;
    std::shared_ptr<BufferBase> buffer;
    std::shared_ptr<DescriptorHandle> handle;
    kBufferType buffer_type;
};

/// <summary>
/// 1つのMaterialBlockが使用するGPUリソースをCBV/SRV/UAVごとにまとめて管理し、DescriptorTableへの登録や更新を行うクラスです。
/// </summary>
class GpuResourceGroup
{
    bool m_is_dirty_ = true;
    std::array<std::map<int, GpuResource>, static_cast<size_t>(kGpuBufferType_Count)> m_gpu_resources_;

    /// <summary>
    /// MaterialBlockのデータが変更されていれば、ConstantBufferに書き込みます。
    /// </summary>
    /// <param name="gpu_resource">更新するリソース</param>
    /// <param name="material_block">データの取得元のMaterialBlock</param>
    static void UpdateConstantBuffer(const GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    /// <summary>
    /// サイズが変わっていればStructuredBufferを作り直し、データが変更されていれば書き込んで、SRVまたはUAVの状態に遷移させます。
    /// </summary>
    /// <param name="gpu_resource">更新するリソース</param>
    /// <param name="material_block">データの取得元のMaterialBlock</param>
    static void UpdateStructuredBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    /// <summary>
    /// Textureが変更されていればBufferを差し替え、SRVの状態に遷移させます。
    /// </summary>
    /// <param name="gpu_resource">更新するリソース</param>
    /// <param name="material_block">データの取得元のMaterialBlock</param>
    static void UpdateTextureBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    /// <summary>
    /// RenderTextureが変更されていればBufferを差し替え、UAVの状態に遷移させます。
    /// </summary>
    /// <param name="gpu_resource">更新するリソース</param>
    /// <param name="material_block">データの取得元のMaterialBlock</param>
    static void UpdateUavTextureBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    /// <summary>
    /// サイズが変わっていればByteAddressBufferを作り直し、データが変更されていれば書き込んで、SRVまたはUAVの状態に遷移させます。
    /// </summary>
    /// <param name="gpu_resource">更新するリソース</param>
    /// <param name="material_block">データの取得元のMaterialBlock</param>
    static void UpdateByteAddressBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);
    /// <summary>
    /// Textureが変更されていればTextureCubeを作り直します。いずれかの面のTextureが未設定の場合はBufferを無効にします。
    /// </summary>
    /// <param name="gpu_resource">更新するリソース</param>
    /// <param name="material_block">データの取得元のMaterialBlock</param>
    static void UpdateTextureCubeBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block);

    /// <summary>
    /// 同じ名前のグローバルなBufferが登録されていれば、それをこのリソースとして使用しViewを作成します。
    /// </summary>
    /// <param name="gpu_resource">設定するリソース</param>
    /// <param name="is_uav">UAVとして作成する場合はtrue</param>
    /// <returns>グローバルなBufferを設定できた場合はtrue</returns>
    static bool SetGlobalResource(GpuResource &gpu_resource, bool is_uav);

public:
    /// <summary>
    /// リソースを追加します。Bufferが未作成の場合は作成します。
    /// </summary>
    /// <param name="buffer">追加するBuffer</param>
    /// <param name="material_data">対応するBufferData</param>
    /// <param name="buffer_type">Bufferの種類</param>
    /// <param name="gpu_upload_type">CBV/SRV/UAVのどれとして渡すか</param>
    void Insert(const std::shared_ptr<BufferBase> &buffer, const std::shared_ptr<BufferDataBase> &material_data, kBufferType buffer_type, kGpuUploadType gpu_upload_type);
    /// <summary>
    /// 指定した種類のリソースが空かどうかを取得します。
    /// </summary>
    /// <param name="buffer_type">CBV/SRV/UAVの種類</param>
    bool Empty(kGpuUploadType buffer_type) const;
    /// <summary>
    /// 指定した種類の先頭のリソースを取得します。DescriptorTableの先頭ハンドルの取得に使用します。
    /// </summary>
    /// <param name="buffer_type">CBV/SRV/UAVの種類</param>
    GpuResource Begin(kGpuUploadType buffer_type);
    /// <summary>
    /// 指定した種類の先頭のリソースを取得します。現状はBeginと同じ要素を返します。
    /// </summary>
    /// <param name="buffer_type">CBV/SRV/UAVの種類</param>
    GpuResource End(kGpuUploadType buffer_type);

    /// <summary>
    /// 指定した名前のリソースのBufferを差し替えます。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <param name="buffer">設定するBuffer</param>
    void SetBuffer(const std::string &name, const std::shared_ptr<BufferBase> &buffer);
    /// <summary>
    /// 指定した名前のリソースのBufferを取得します。
    /// </summary>
    /// <param name="name">Shader上のBuffer名</param>
    /// <returns>見つからない場合はnullptr</returns>
    std::shared_ptr<BufferBase> GetBuffer(const std::string &name);

    /// <summary>
    /// すべてのリソースをMaterialBlockのデータで更新します。グローバルなBufferが登録されている名前はそちらを優先します。
    /// </summary>
    /// <param name="material_block">データの取得元のMaterialBlock</param>
    bool UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block);
    /// <summary>
    /// ハンドル未割り当てのリソースがある種類について連続したDescriptorを確保し、各リソースのViewを作成します。
    /// </summary>
    /// <returns>すべてのリソースを登録できた場合はtrue。Bufferが未設定のリソースがある場合はfalse</returns>
    bool SetBufferToDescriptorTable();
    /// <summary>
    /// UAVの各リソースにUAVバリアを積み、書き込みの完了を待つようにします。
    /// </summary>
    void WaitUavWrite();
};
}