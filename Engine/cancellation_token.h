#pragma once

/// <summary>
/// CancellationTokenSourceによってキャンセルが要求されたかどうかを確認するためのトークンです。
/// </summary>
class CancellationToken
{
    friend class CancellationTokenSource;

    /// <summary>
    /// TokenとSourceの間で共有されるキャンセル状態です。
    /// </summary>
    struct State
    {
        std::atomic_bool cancelled;
    };

    std::shared_ptr<State> m_state_;

    /// <summary>
    /// 共有されたキャンセル状態からTokenを生成します。CancellationTokenSourceからのみ呼び出されます。
    /// </summary>
    explicit CancellationToken(std::shared_ptr<State> state);

public:
    CancellationToken() = default;
    /// <summary>
    /// キャンセルが要求されたかどうかを取得します。
    /// </summary>
    /// <returns>キャンセルが要求されていればtrue。状態を持たないTokenの場合は常にfalse</returns>
    [[nodiscard]] bool IsCancellationRequested() const;
};