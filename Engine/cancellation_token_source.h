#pragma once
#include "cancellation_token.h"

class CancellationTokenSource
{
    std::shared_ptr<CancellationToken::State> m_state_;

public:
    CancellationTokenSource();

    CancellationToken GetToken() const;
    void Cancel() const;
};