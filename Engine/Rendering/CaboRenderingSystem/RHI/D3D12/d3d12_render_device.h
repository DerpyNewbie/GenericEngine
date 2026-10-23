#pragma once
#include <array>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/slot_pool.h"
#include "Rendering/CaboRenderingSystem/RHI/i_render_device.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_common.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_descriptor_heap.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_shader_compiler.h"

namespace CRS
{
class D3D12CommandList;

/// <summary>
/// 共通ルートシグネチャのパラメーター番号です。定数はルートCBV、t / s はディスクリプタテーブルで渡します。
/// </summary>
enum D3D12RootParameter : uint32_t
{
    kRootParameter_Constants = 0, // b0..b3, space0（kConstantSlotCount個続く）
    kRootParameter_FrameConstants = kRootParameter_Constants + kConstantSlotCount, // b0..b2, space1
    kRootParameter_Resources = kRootParameter_FrameConstants + kFrameConstantSlotCount, // t0..t7, space0
    kRootParameter_FrameResources, // t0..t7, space1
    kRootParameter_Samplers, // s0..s3, space0
    kRootParameter_Count
};

/// <summary>
/// BufferHandleが指すバッファの本体です。
/// </summary>
struct D3D12Buffer
{
    BufferDesc desc;
    // Uploadメモリの場合はフレーム数分のコピーを持つ。それ以外は[0]だけを使う
    std::array<ComPtr<ID3D12Resource>, kMaxFramesInFlight> resources;
    // Uploadメモリの場合の、CPUから書き込めるアドレス（Mapしたまま）
    std::array<uint8_t *, kMaxFramesInFlight> mapped = {};
    std::array<CpuDescriptor, kMaxFramesInFlight> srv;
    ResourceState state = ResourceState::Common;
    // Uploadメモリの場合の最新の内容。他のフレームのコピーへ反映するために使う
    std::vector<uint8_t> shadow;
    // まだ最新の内容を反映していないフレームのビット（1 << frame_index）
    uint32_t pending_frame_mask = 0;

    /// <summary>
    /// フレームごとのコピーを持つ（Uploadメモリの）バッファかどうかです。
    /// </summary>
    bool IsVersioned() const
    {
        return desc.memory == MemoryType::Upload;
    }
};

/// <summary>
/// TextureHandleが指すテクスチャの本体と、そのビューです。
/// </summary>
struct D3D12Texture
{
    TextureDesc desc;
    ComPtr<ID3D12Resource> resource;
    ResourceState state = ResourceState::Common;
    // 外部から取り込んだリソースで、状態を変えない場合true
    bool fixed_state = false;
    CpuDescriptor srv;
    CpuDescriptor rtv;
    // 全体を指すDSV
    CpuDescriptor dsv;
    // 配列テクスチャのスライスごとのDSV
    std::vector<CpuDescriptor> dsv_slices;
};

/// <summary>
/// ShaderHandleが指す、コンパイル済みのDXILとリフレクションです。
/// </summary>
struct D3D12Shader
{
    std::vector<uint8_t> bytecode;
    ShaderReflection reflection;
};

/// <summary>
/// PipelineStateHandleが指すPSOです。キャッシュで同じ設定か比べるためにdescも持ちます。
/// </summary>
struct D3D12PipelineState
{
    PipelineStateDesc desc;
    ComPtr<ID3D12PipelineState> pipeline_state;
};

/// <summary>
/// SamplerHandleが指すサンプラーです（シェーダーから見えないヒープに置いたもの）。
/// </summary>
struct D3D12Sampler
{
    SamplerDesc desc;
    CpuDescriptor descriptor;
};

/// <summary>
/// IRenderDeviceのD3D12実装です。D3D12を知るのはRHI/D3D12の中だけです。
/// </summary>
class D3D12RenderDevice final : public IRenderDevice
{
    friend class D3D12CommandList;

    // アップロード用のリングの1ページ。先頭から順に切り出し、そのフレームのGPU処理が終わったら巻き戻す
    struct UploadPage
    {
        ComPtr<ID3D12Resource> resource;
        uint8_t *mapped = nullptr;
        uint64_t size = 0;
        // 次に切り出す位置
        uint64_t offset = 0;
    };

    // 読み戻し中のバッファ。fence_valueまでのGPU処理が終わればstagingを読める
    struct PendingReadback
    {
        ComPtr<ID3D12Resource> staging;
        uint64_t size = 0;
        uint64_t fence_value = 0;
    };

    // アップロード用のリングの1ページの大きさ。これより大きい確保は専用のページを作る
    static constexpr uint64_t kUploadPageSize = 4ull * 1024 * 1024;

    ComPtr<IDXGIFactory4> m_factory_;
    ComPtr<ID3D12Device6> m_device_;
    ComPtr<ID3D12CommandQueue> m_queue_;
    ComPtr<IDXGISwapChain3> m_swap_chain_;
    std::array<ComPtr<ID3D12CommandAllocator>, kMaxFramesInFlight> m_allocators_;
    std::unique_ptr<D3D12CommandList> m_command_list_;

    // 読み込み時のアップロード用。フレームのフェンス値を消費しないよう、別のフェンスを使う
    ComPtr<ID3D12CommandAllocator> m_immediate_allocator_;
    ComPtr<ID3D12GraphicsCommandList> m_immediate_list_;
    ComPtr<ID3D12Fence> m_immediate_fence_;
    uint64_t m_immediate_fence_value_ = 0;

    ComPtr<ID3D12Fence> m_fence_;
    HANDLE m_fence_event_ = nullptr;
    uint64_t m_next_fence_value_ = 1;
    std::array<uint64_t, kMaxFramesInFlight> m_frame_fence_values_ = {};
    uint32_t m_frame_index_ = 0;
    bool m_frame_open_ = false;

    ComPtr<ID3D12RootSignature> m_root_signature_;

    CpuDescriptorAllocator m_cpu_srv_heap_;
    CpuDescriptorAllocator m_cpu_rtv_heap_;
    CpuDescriptorAllocator m_cpu_dsv_heap_;
    CpuDescriptorAllocator m_cpu_sampler_heap_;
    GpuDescriptorRing m_gpu_srv_ring_;
    GpuDescriptorRing m_gpu_sampler_ring_;
    CpuDescriptor m_null_srv_;
    CpuDescriptor m_default_sampler_;

    std::array<std::vector<UploadPage>, kMaxFramesInFlight> m_upload_pages_;
    std::array<std::vector<std::function<void()>>, kMaxFramesInFlight> m_deferred_releases_;
    std::vector<BufferHandle> m_pending_buffers_;

    SlotPool<BufferTag, D3D12Buffer> m_buffers_;
    SlotPool<TextureTag, D3D12Texture> m_textures_;
    SlotPool<ShaderTag, D3D12Shader> m_shaders_;
    SlotPool<PipelineStateTag, D3D12PipelineState> m_pipeline_states_;
    SlotPool<SamplerTag, D3D12Sampler> m_samplers_;
    std::unordered_multimap<size_t, PipelineStateHandle> m_pipeline_state_cache_;

    std::array<TextureHandle, kMaxFramesInFlight> m_back_buffers_ = {};
    Format m_back_buffer_format_ = Format::R8G8B8A8_UNorm;

    D3D12ShaderCompiler m_shader_compiler_;
    ShaderReflection m_empty_reflection_;

    uint64_t m_next_readback_id_ = 1;
    std::unordered_map<uint64_t, PendingReadback> m_readbacks_;

    D3D12RenderDevice() = default;

    // デバイス、キュー、スワップチェーン、ヒープ、ルートシグネチャ、バックバッファを順に作る
    bool Init(HWND hwnd, uint32_t width, uint32_t height);
    // 全シェーダー共通のルートシグネチャを作る（割り当てはrhi_types.hのコメントを参照）
    bool CreateRootSignature();
    // スワップチェーンのバッファをテクスチャとして登録する（m_back_buffers_）
    bool CreateBackBuffers();
    // Resizeの前に、スワップチェーンのバッファへの参照をすべて手放す
    void ReleaseBackBuffers();

    // フェンスがvalueに届くまでCPUを止める
    void WaitForFence(uint64_t value);
    // frame_indexのGPU処理が終わった後の片付け。遅延破棄の実行、アップロード用のリングの巻き戻し、
    // Uploadバッファの最新の内容をこのフレームのコピーに反映する
    void RetireFrame(uint32_t frame_index);
    // 今のフレームのGPU処理が終わった後（同じフレーム番号のRetireFrame）にreleaseを呼ぶ
    void DeferRelease(std::function<void()> release);

    // 用途に合わせてSRV / RTV / DSV（配列ならスライスごとのDSVも）を作る
    void CreateTextureViews(D3D12Texture &texture);
    // テクスチャの形（2D / 配列 / キューブ / MSAA）に合わせたSRVの設定を作る。
    // 深度テクスチャはformatに読めるフォーマット（R32_FLOAT）を渡す
    static D3D12_SHADER_RESOURCE_VIEW_DESC MakeSrvDesc(DXGI_FORMAT format, TextureDimension dimension,
                                                       uint32_t array_size, bool multisampled, uint32_t mip_levels);
    // StructuredBufferのSRVを、フレームごとのコピーの数だけ作る
    void CreateBufferViews(D3D12Buffer &buffer);
    // テクスチャのビューのスロットを返す（遅延破棄の中で呼ぶ）
    void ReleaseTextureViews(D3D12Texture &texture);

    // 現在のフレームのアップロード用リングから確保します。CPUで書き込むアドレスと、GPUから読むアドレスを返します
    bool AllocateUpload(uint64_t size, uint64_t alignment, uint8_t *&out_cpu, D3D12_GPU_VIRTUAL_ADDRESS &out_gpu);
    // バッファのGPUアドレス。Uploadなら今のフレームのコピーのアドレス
    D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(BufferHandle buffer);
    // ResourceBindingが指すもののSRV。何も指していなければ空のSRV（読むと0）
    D3D12_CPU_DESCRIPTOR_HANDLE GetResourceSrv(const ResourceBinding &binding);
    // サンプラーのディスクリプタ。無効なハンドルなら既定のサンプラー（Linear / Wrap）
    D3D12_CPU_DESCRIPTOR_HANDLE GetSamplerDescriptor(SamplerHandle sampler);

public:
    /// <summary>
    /// デバイス、スワップチェーン、共通ルートシグネチャなどを作成します。失敗した場合はnullptrを返します。
    /// </summary>
    static std::unique_ptr<D3D12RenderDevice> Create(HWND hwnd, uint32_t width, uint32_t height);
    /// <summary>
    /// GPUの処理がすべて終わるまで待ってから、残っているリソースを解放します。
    /// </summary>
    ~D3D12RenderDevice() override;

    // ---- IRenderDeviceの実装（説明はIRenderDeviceを参照） ----
    BufferHandle CreateBuffer(const BufferDesc &desc) override;
    TextureHandle CreateTexture(const TextureDesc &desc) override;
    ShaderHandle CreateShader(const ShaderSource &source) override;
    PipelineStateHandle CreatePipelineState(const PipelineStateDesc &desc) override;
    SamplerHandle CreateSampler(const SamplerDesc &desc) override;

    const ShaderReflection &GetReflection(ShaderHandle shader) const override;
    TextureDesc GetTextureDesc(TextureHandle texture) const override;

    void Destroy(BufferHandle buffer) override;
    void Destroy(TextureHandle texture) override;
    void Destroy(ShaderHandle shader) override;
    void Destroy(PipelineStateHandle pipeline_state) override;
    void Destroy(SamplerHandle sampler) override;

    void UpdateBuffer(BufferHandle buffer, const void *data, uint64_t size, uint64_t offset = 0) override;

    ReadbackTicket RequestReadback(BufferHandle buffer) override;
    bool TryGetReadback(ReadbackTicket ticket, std::vector<uint8_t> &out_data) override;

    ICommandList &BeginCommandList() override;
    void Submit(ICommandList &command_list) override;

    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;
    TextureHandle GetBackBuffer() const override;
    Format GetBackBufferFormat() const override;
    uint32_t GetFrameIndex() const override;
    void Resize(uint32_t width, uint32_t height) override;
    void WaitIdle() override;

    // ---- D3D12固有。RHI/D3D12と、エンジン側のつなぎ（CrsBridge、ImGui）からだけ使います ----
    /// <summary>
    /// D3D12のデバイスです（ImGuiの初期化など）。
    /// </summary>
    ID3D12Device6 *NativeDevice() const;
    /// <summary>
    /// コマンドを送るキューです。
    /// </summary>
    ID3D12CommandQueue *NativeQueue() const;
    /// <summary>
    /// 現在のフレームのコマンドリストです（BeginFrameとSubmitの間だけ記録可能）。
    /// 直接記録した後は、D3D12CommandList::InvalidateStateで覚えている設定を捨ててください。
    /// </summary>
    ID3D12GraphicsCommandList *NativeCommandList() const;
    /// <summary>
    /// テクスチャのD3D12リソースです。無効なハンドルならnullptrを返します。
    /// </summary>
    ID3D12Resource *NativeResource(TextureHandle texture);
    /// <summary>
    /// テクスチャを描画先にするときのRTVです（OMSetRenderTargetsに直接渡す場合）。
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetRtv(TextureHandle texture);
    /// <summary>
    /// テクスチャを深度バッファにするときのDSVです。配列テクスチャならsliceで1枚を選べます（kAllSlicesなら全体）。
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsv(TextureHandle texture, uint32_t slice = kAllSlices);
    /// <summary>
    /// 外部で作成したテクスチャを取り込みます。状態はfixed_stateのまま変わらない前提です。
    /// </summary>
    TextureHandle ImportTexture(ID3D12Resource *resource, ResourceState fixed_state,
                                TextureDimension dimension = TextureDimension::Texture2D);
    /// <summary>
    /// 次にSignalで使うフェンスの値です。今記録しているコマンドが終わったかを、後でIsFenceCompleteで確かめるのに使います。
    /// </summary>
    uint64_t NextFenceValue() const;
    /// <summary>
    /// fence_valueまでのGPU処理が終わっているかどうかです（待ちません）。
    /// </summary>
    bool IsFenceComplete(uint64_t fence_value) const;
    /// <summary>
    /// フレームのコマンドリストが記録可能（BeginFrameとSubmitの間）かどうかです。
    /// </summary>
    bool IsFrameOpen() const
    {
        return m_frame_open_;
    }
    /// <summary>
    /// 読み込み時用。専用のコマンドリストで記録して実行し、完了まで待ちます。
    /// </summary>
    void ExecuteImmediate(const std::function<void(ID3D12GraphicsCommandList *)> &record);

    // ---- 外部のD3D12ライブラリ（ImGuiなど）用 ----
    /// <summary>
    /// シェーダーから見えるヒープに、ずっと使い続けるSRVのスロットを確保します（ImGuiのフォントなど）。
    /// </summary>
    GpuDescriptorRange AllocatePersistentSrv();
    /// <summary>
    /// シェーダーから見えるCBV/SRV/UAVのヒープです（ImGuiの初期化など）。
    /// </summary>
    ID3D12DescriptorHeap *NativeSrvHeap() const
    {
        return m_gpu_srv_ring_.Heap();
    }
    /// <summary>
    /// AllocatePersistentSrvで確保したスロットを返します。
    /// </summary>
    void FreePersistentSrv(const GpuDescriptorRange &range);
    /// <summary>
    /// テクスチャのSRVを、このフレームだけ有効なスロットに置き、GPUハンドルを返します（ImGui::Imageなど）。
    /// </summary>
    D3D12_GPU_DESCRIPTOR_HANDLE GetTransientSrv(TextureHandle texture);
    /// <summary>
    /// CRSのシェーダーから見えるヒープをコマンドリストに設定します。外部のライブラリで描く前に呼びます。
    /// </summary>
    void BindDescriptorHeaps(ID3D12GraphicsCommandList *list) const;
};
}
