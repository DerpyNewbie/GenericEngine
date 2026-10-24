#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/i_command_list.h"
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"
#include "Rendering/CaboRenderingSystem/RHI/shader_reflection.h"

namespace CRS
{
/// <summary>
/// GPUリソースの作成・破棄とフレームの制御を行うInterfaceです。GPUの実体にはこれを経由してしか触りません。
/// <para>1フレームの流れ: BeginFrame → BeginCommandListで記録 → Submit → EndFrame → Present</para>
/// </summary>
class IRenderDevice
{
public:
    virtual ~IRenderDevice() = default;

    // ---- リソース作成 ----
    /// <summary>
    /// バッファを作成します。GpuOnlyでinitial_dataがあれば、作成時にアップロードします。失敗した場合は無効なハンドルを返します。
    /// </summary>
    virtual BufferHandle CreateBuffer(const BufferDesc &desc) = 0;
    /// <summary>
    /// テクスチャを作成し、用途に合わせたビュー（SRV / RTV / DSV）も作ります。失敗した場合は無効なハンドルを返します。
    /// </summary>
    virtual TextureHandle CreateTexture(const TextureDesc &desc) = 0;
    /// <summary>
    /// HLSLをコンパイルし、リフレクション結果も保持します。失敗した場合は無効なハンドルを返します。
    /// </summary>
    virtual ShaderHandle CreateShader(const ShaderSource &source) = 0;
    /// <summary>
    /// PSOを作成します。同じ設定のPSOはキャッシュから返すので、描画のたびに呼んでかまいません。
    /// </summary>
    virtual PipelineStateHandle CreatePipelineState(const PipelineStateDesc &desc) = 0;
    /// <summary>
    /// サンプラーを作成します。
    /// </summary>
    virtual SamplerHandle CreateSampler(const SamplerDesc &desc) = 0;

    /// <summary>
    /// シェーダーのリフレクション（cbufferの変数、リソースのレジスタ）を取得します。無効なハンドルなら空の結果を返します。
    /// </summary>
    virtual const ShaderReflection &GetReflection(ShaderHandle shader) const = 0;
    /// <summary>
    /// テクスチャの作成時の設定を取得します（大きさの確認など）。initial_dataは含みません。
    /// </summary>
    virtual TextureDesc GetTextureDesc(TextureHandle texture) const = 0;

    // ---- 破棄 ----
    // すぐには解放せず、GPUがそのリソースを使い終わったフレームで解放します。ハンドルはすぐに無効になります
    virtual void Destroy(BufferHandle buffer) = 0;
    virtual void Destroy(TextureHandle texture) = 0;
    virtual void Destroy(ShaderHandle shader) = 0;
    virtual void Destroy(PipelineStateHandle pipeline_state) = 0;
    virtual void Destroy(SamplerHandle sampler) = 0;

    /// <summary>
    /// Uploadメモリのバッファに書き込みます。書き込み先は現在のフレームのコピーで、
    /// 他のフレームのコピーにはそのフレームのGPU処理が終わってから反映します。
    /// </summary>
    virtual void UpdateBuffer(BufferHandle buffer, const void *data, uint64_t size, uint64_t offset = 0) = 0;

    // ---- ReadBack（チケット方式） ----
    /// <summary>
    /// GPUのバッファの内容をCPUに読み戻すよう依頼します。結果はTryGetReadbackで受け取ります。
    /// </summary>
    virtual ReadbackTicket RequestReadback(BufferHandle buffer) = 0;
    /// <summary>
    /// 読み戻しが終わっていればout_dataに結果を入れてtrueを返します。まだならfalseを返すので、次のフレームで再度呼びます。
    /// </summary>
    virtual bool TryGetReadback(ReadbackTicket ticket, std::vector<uint8_t> &out_data) = 0;

    // ---- コマンド ----
    /// <summary>
    /// そのフレームのコマンドリストを返します。BeginFrameとSubmitの間で使います。
    /// </summary>
    virtual ICommandList &BeginCommandList() = 0;
    /// <summary>
    /// 記録したコマンドリストを閉じてGPUに送ります。この後はEndFrameまで記録できません。
    /// </summary>
    virtual void Submit(ICommandList &command_list) = 0;

    // ---- フレーム制御 ----
    /// <summary>
    /// フレームを始めます。コマンドリストとディスクリプタのリングを、このフレーム用にリセットします。
    /// </summary>
    virtual void BeginFrame() = 0;
    /// <summary>
    /// フレームの終わりを記録します（フェンスを発行し、このフレームのGPU処理の完了を追えるようにします）。
    /// </summary>
    virtual void EndFrame() = 0;
    /// <summary>
    /// 画面に表示し、次のフレームへ進みます。次に使うバックバッファの前回のGPU処理が終わるまで待ち、
    /// 遅延破棄やアップロード用のリングを回収します。
    /// </summary>
    virtual void Present() = 0;
    /// <summary>
    /// 今のフレームで描くバックバッファです。フレームごとに変わるので、毎フレーム取得してください。
    /// </summary>
    virtual TextureHandle GetBackBuffer() const = 0;
    /// <summary>
    /// バックバッファのフォーマットです（PSOの描画先の設定に使います）。
    /// </summary>
    virtual Format GetBackBufferFormat() const = 0;
    /// <summary>
    /// 今のフレームの番号（0..kMaxFramesInFlight-1）です。フレームごとのコピーを選ぶのに使います。
    /// </summary>
    virtual uint32_t GetFrameIndex() const = 0;
    /// <summary>
    /// ウィンドウの大きさに合わせてバックバッファを作り直します。GPUの処理がすべて終わるまで待ちます。
    /// </summary>
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    /// <summary>
    /// GPUの処理がすべて終わるまで待ちます（終了時や作り直しの前に使います）。
    /// </summary>
    virtual void WaitIdle() = 0;
};
}
