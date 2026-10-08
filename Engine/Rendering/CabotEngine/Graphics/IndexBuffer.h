#pragma once
namespace engine
{
/// <summary>
/// Meshのindexを持つBufferです。
/// </summary>
class IndexBuffer
{
    //TODO : upload_resourceを保持する必要があるか？初期化後は不要なはず、ExecuteCommandListの後にResetしましょう
    ComPtr<ID3D12Resource> m_buffer_;
    D3D12_INDEX_BUFFER_VIEW m_view_;
    
public:
    /// <summary>
    /// Indexのデータを書き込んだBufferを作成します。
    /// </summary>
    /// <param name="size">データのサイズ(byte)</param>
    /// <param name="init_data">Indexのデータ</param>
    explicit IndexBuffer(size_t size, const uint32_t *init_data);
    IndexBuffer(const IndexBuffer &) = delete;
    void operator =(const IndexBuffer &) = delete;

    /// <summary>
    /// Bufferが作成済みであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsValid() const;
    /// <summary>
    /// IndexBufferViewを取得します。
    /// </summary>
    D3D12_INDEX_BUFFER_VIEW *View();

};
}