#pragma once

namespace engine
{
/// <summary>
/// BufferをGPUにどのViewとして渡すかの種類です。
/// </summary>
enum kGpuUploadType : int
{
    kGpuBufferType_CBV = 0,
    kGpuBufferType_SRV = 1,
    kGpuBufferType_UAV = 2,

    kGpuBufferType_Count = 3

};

class DescriptorHandle;

/// <summary>
/// GPU上のBufferリソースを扱うクラスの基底クラスです。
/// </summary>
class BufferBase
{
public:
    bool is_dirty;

    virtual ~BufferBase() = default;
    /// <summary>
    /// GPU上にBufferのリソースを作成します。
    /// </summary>
    virtual void CreateBuffer() = 0;
    /// <summary>
    /// CPU側のデータをBufferに書き込みます。
    /// </summary>
    /// <param name="data">書き込むデータ</param>
    virtual void UpdateBuffer(const void *data) = 0;
    /// <summary>
    /// 指定したDescriptorHandleの位置に、このBufferのViewを作成します。
    /// </summary>
    /// <param name="desc_handle">Viewを作成するDescriptorHandle</param>
    /// <param name="is_uav">UAVとして作成する場合はtrue</param>
    virtual void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) = 0;
    /// <summary>
    /// このBufferをGPUに渡す際のViewの種類を取得します。
    /// </summary>
    virtual kGpuUploadType BufferType() const = 0;
    /// <summary>
    /// Bufferのリソースが作成済みで使用可能かどうかを取得します。
    /// </summary>
    virtual bool IsValid() = 0;
    /// <summary>
    /// リソースの状態を遷移させるバリアをCommandListに積みます。
    /// </summary>
    /// <param name="new_state">遷移先の状態</param>
    /// <returns>遷移を行った場合はtrue。既に同じ状態の場合はfalse</returns>
    virtual bool Transition(D3D12_RESOURCE_STATES new_state) = 0;
    /// <summary>
    /// GPUリソースを取得します。
    /// </summary>
    virtual ID3D12Resource *Resource() = 0;
    /// <summary>
    /// GPUからCPUへのBufferの読み戻しを要求します。結果はFetchBufferDataで取得します。
    /// </summary>
    virtual void RequestReadBack() = 0;
    /// <summary>
    /// RequestReadBackで要求した読み戻しの結果を取得します。
    /// </summary>
    /// <param name="data">読み戻したデータの書き込み先</param>
    /// <returns>GPUの処理が完了しデータを取得できた場合はtrue</returns>
    virtual bool FetchBufferData(void *data) = 0;
};
}