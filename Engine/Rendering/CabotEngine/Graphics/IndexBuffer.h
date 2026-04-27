#pragma once
namespace engine
{
class IndexBuffer
{
    ComPtr<ID3D12Resource> m_p_resource_;
    D3D12_INDEX_BUFFER_VIEW m_view_;

    IndexBuffer(const IndexBuffer &);
    void operator =(const IndexBuffer &) = delete;
    
public:
    explicit IndexBuffer(size_t size, const uint32_t *p_init_data = nullptr);

    [[nodiscard]] bool IsValid() const;
    D3D12_INDEX_BUFFER_VIEW *View();

};
}