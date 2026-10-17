#pragma once
#include "Rendering/buffer_base.h"
#include "Rendering/shader_resource.h"

namespace engine
{
/// <summary>
/// GPU上のByteAddressBuffer(RWByteAddressBuffer)を管理するクラスです。要素は4byte単位で扱います。
/// </summary>
class ByteAddressBuffer : public BufferBase
{
    ComPtr<ID3D12Resource> m_default_resource_;
    ComPtr<ID3D12Resource> m_upload_resource_;
    ComPtr<ID3D12Resource> m_readback_resource_;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address_;
    size_t m_element_count_ = 0;
    D3D12_RESOURCE_STATES m_current_state_ = D3D12_RESOURCE_STATE_COMMON;
    uint64_t m_read_back_fence_value_;

    /// <summary>
    /// GPUからの読み戻し用のReadbackリソースを作成します。
    /// </summary>
    void CreateReadBackResource();

public:
    /// <summary>
    /// リソースをDirectXResourceFactoryの解放待ちに登録し、GPUの使用が終わってから解放されるようにします。
    /// </summary>
    ~ByteAddressBuffer() override;

    /// <summary>
    /// 指定した要素数(4byte単位)でBufferを生成します。リソースはCreateBufferで作成されます。
    /// </summary>
    /// <param name="elem_count">要素数</param>
    ByteAddressBuffer(size_t elem_count);

    /// <summary>
    /// UAVを許可したDefaultヒープのリソースと、転送用のUploadヒープのリソースを作成します。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// データをUploadリソース経由でDefaultリソースにコピーするコマンドを積みます。
    /// </summary>
    /// <param name="data">要素数分のデータ</param>
    void UpdateBuffer(const void *data) override;
    /// <summary>
    /// 指定したDescriptorHandleの位置にRawバッファのSRVまたはUAVを作成します。
    /// </summary>
    /// <param name="desc_handle">Viewを作成するDescriptorHandle</param>
    /// <param name="is_uav">UAVとして作成する場合はtrue</param>
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    /// <summary>
    /// SRVとして扱うため、kGpuBufferType_SRVを返します。
    /// </summary>
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_SRV;
    }
    /// <summary>
    /// Defaultリソースが作成済みかどうかを取得します。
    /// </summary>
    bool IsValid() override;
    /// <summary>
    /// Defaultリソースの状態を遷移させるバリアを積みます。
    /// </summary>
    /// <param name="new_state">遷移先の状態</param>
    /// <returns>遷移を行った場合はtrue</returns>
    bool Transition(D3D12_RESOURCE_STATES new_state) override;

    /// <summary>
    /// Readbackリソースへのコピーを積み、読み戻し完了判定用のFence値を記録します。
    /// </summary>
    void RequestReadBack() override;
    /// <summary>
    /// 読み戻しのFenceが完了していれば、Readbackリソースの内容をコピーします。
    /// </summary>
    /// <param name="data">コピー先</param>
    /// <returns>コピーできた場合はtrue</returns>
    bool FetchBufferData(void *data) override;

    /// <summary>
    /// RawバッファとしてのUAVの設定を取得します。
    /// </summary>
    [[nodiscard]] D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc() const;
    /// <summary>
    /// RawバッファとしてのSRVの設定を取得します。
    /// </summary>
    [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();
    /// <summary>
    /// Defaultリソースを取得します。
    /// </summary>
    [[nodiscard]] ID3D12Resource *Resource() override;

    /// <summary>
    /// DefaultリソースのGPU仮想アドレスを取得します。
    /// </summary>
    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
};
}