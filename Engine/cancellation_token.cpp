#include "pch.h"
#include "cancellation_token.h"

CancellationToken::CancellationToken(std::shared_ptr<State> state)
    : m_state_(std::move(state))
{
}

bool CancellationToken::IsCancellationRequested() const
{
    return m_state_ && m_state_->cancelled.load(std::memory_order_acquire);
}
