#pragma once
#include "RenderEngine.h"
#include "Rendering/buffer_base.h"

namespace engine
{
/// <summary>
/// 定数バッファを管理するクラスです。フレームバッファ数分のUploadリソースを持ち、常時Mapしたまま使用します。
/// </summary>
class ConstantBuffer : public BufferBase
{
    uint64_t m_size_aligned_;
    uint64_t m_size_;

    bool m_is_first_update_ = true;
    std::array<ComPtr<ID3D12Resource>, RenderEngine::kFrame_Buffer_Count> m_resources_;
    std::array<D3D12_CONSTANT_BUFFER_VIEW_DESC, RenderEngine::kFrame_Buffer_Count> m_desc_;
    std::array<D3D12_RESOURCE_STATES, RenderEngine::kFrame_Buffer_Count> m_current_state_;

    std::array<void *, RenderEngine::kFrame_Buffer_Count> m_p_mapped_ptrs_ = {nullptr};

public:
    /// <summary>
    /// リソースをDirectXResourceFactoryの解放待ちに登録し、GPUの使用が終わってから解放されるようにします。
    /// </summary>
    ~ConstantBuffer() override;
    /// <summary>
    /// 指定したサイズで定数バッファを生成します。リソースはCreateBufferで作成されます。
    /// </summary>
    /// <param name="size">データのサイズ(byte)</param>
    explicit ConstantBuffer(size_t size);

    /// <summary>
    /// データのサイズを設定し、256byteにアラインしたリソースのサイズを計算します。
    /// </summary>
    /// <param name="size">データのサイズ(byte)</param>
    void SetBufferSize(const size_t size)
    {
        m_size_ = size;
        constexpr size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        m_size_aligned_ = ((size + (align - 1)) & ~(align - 1));
    }

    /// <summary>
    /// フレームバッファ数分のリソースを作成してMapし、CBVの設定を準備します。
    /// </summary>
    void CreateBuffer() override;
    /// <summary>
    /// 現在のバックバッファに対応するリソースにデータを書き込みます。初回のみ全フレーム分のリソースに書き込みます。
    /// </summary>
    /// <param name="data">書き込むデータ</param>
    void UpdateBuffer(const void *data) override;
    /// <summary>
    /// 指定したDescriptorHandleの位置に、現在のフレームのCBVを作成します。is_uavは無視されます。
    /// </summary>
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    /// <summary>
    /// kGpuBufferType_CBVを返します。
    /// </summary>
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_CBV;
    }
    /// <summary>
    /// リソースが作成済みかどうかを取得します。
    /// </summary>
    bool IsValid() override;
    /// <summary>
    /// 現在のフレームのリソースの状態を遷移させるバリアを積みます。
    /// </summary>
    /// <param name="new_state">遷移先の状態</param>
    /// <returns>遷移を行った場合はtrue</returns>
    bool Transition(D3D12_RESOURCE_STATES new_state) override;

    /// <summary>
    /// 定数バッファは読み戻しに対応していないため、警告のみ出力します。
    /// </summary>
    void RequestReadBack() override;
    /// <summary>
    /// 定数バッファは読み戻しに対応していないため、警告を出力してfalseを返します。
    /// </summary>
    bool FetchBufferData(void *data) override;

    /// <summary>
    /// 現在のフレームのリソースのMap済みポインタを取得します。
    /// </summary>
    void *GetPtr() const;

    /// <summary>
    /// 現在のフレームのリソースのGPU仮想アドレスを取得します。
    /// </summary>
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    /// <summary>
    /// 現在のフレームのCBVの設定を取得します。
    /// </summary>
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc() const;
    /// <summary>
    /// 現在のフレームのリソースを取得します。
    /// </summary>
    ID3D12Resource *Resource() override;

    /// <summary>
    /// 現在のフレームのリソースのMap済みポインタを指定した型で取得します。
    /// </summary>
    template <typename T>
    T *GetPtr()
    {
        return static_cast<T *>(GetPtr());
    }
};
}