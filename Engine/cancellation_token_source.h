#pragma once
#include "cancellation_token.h"

/// <summary>
/// CancellationTokenを発行し、キャンセルを通知するクラスです。
/// </summary>
class CancellationTokenSource
{
    std::shared_ptr<CancellationToken::State> m_state_;

public:
    /// <summary>
    /// 新しいキャンセル状態を持つSourceを生成します。
    /// </summary>
    CancellationTokenSource();

    /// <summary>
    /// このSourceのキャンセル状態を共有するTokenを取得します。
    /// </summary>
    CancellationToken GetToken() const;
    /// <summary>
    /// キャンセルを要求し、発行済みのすべてのTokenに通知します。
    /// </summary>
    void Cancel() const;
};