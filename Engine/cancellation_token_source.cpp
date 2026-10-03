#include "pch.h"
#include "cancellation_token_source.h"

CancellationTokenSource::CancellationTokenSource()
    : m_state_(std::make_shared<CancellationToken::State>())
{
}

CancellationToken CancellationTokenSource::GetToken() const
{
    return CancellationToken(m_state_);
}

void CancellationTokenSource::Cancel() const
{
    m_state_->cancelled.store(true, std::memory_order_release);
}
