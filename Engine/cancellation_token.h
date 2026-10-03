#pragma once

class CancellationToken
{
    friend class CancellationTokenSource;

    struct State
    {
        std::atomic_bool cancelled;
    };

    std::shared_ptr<State> m_state_;

    explicit CancellationToken(std::shared_ptr<State> state);

public:
    CancellationToken() = default;
    [[nodiscard]] bool IsCancellationRequested() const;
};
