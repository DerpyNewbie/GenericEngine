#pragma once
namespace engine
{
class IndexBuffer
{
    //TODO : upload_resourceを保持する必要があるか？初期化後は不要なはず、ExecuteCommandListの後にResetしましょう
    ComPtr<ID3D12Resource> m_upload_resource_;
    ComPtr<ID3D12Resource> m_default_resource_;
    D3D12_INDEX_BUFFER_VIEW m_view_;
    
public:
    explicit IndexBuffer(size_t size, const uint32_t *init_data = nullptr);
    IndexBuffer(const IndexBuffer &) = delete;
    void operator =(const IndexBuffer &) = delete;

    [[nodiscard]] bool IsValid() const;
    D3D12_INDEX_BUFFER_VIEW *View();

};
}