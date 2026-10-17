#pragma once

/// <summary>
/// 呼び出しの順番(Order)を持つInterfaceです。
/// </summary>
class IOrderable
{
public:
    virtual ~IOrderable() = default;
    /// <summary>
    /// 呼び出される順番を取得します。値が小さいほど先に呼ばれます。
    /// </summary>
    virtual int Order()
    {
        return 0;
    }
};

/// <summary>
/// UpdateManagerからUpdateの通知を受け取るInterfaceです。
/// </summary>
class IUpdateReceiver : public IOrderable
{
public:
    /// <summary>
    /// 毎フレームのUpdateで呼ばれます。
    /// </summary>
    virtual void OnUpdate() = 0;
};

/// <summary>
/// UpdateManagerからFixedUpdateの通知を受け取るInterfaceです。
/// </summary>
class IFixedUpdateReceiver : public IOrderable
{
public:
    /// <summary>
    /// FixedUpdateで呼ばれます。
    /// </summary>
    virtual void OnFixedUpdate() = 0;
};

/// <summary>
/// UpdateManagerからRenderの通知を受け取るInterfaceです。
/// </summary>
class IRenderReceiver : public IOrderable
{
public:
    /// <summary>
    /// 毎フレームの描画のタイミングで呼ばれます。
    /// </summary>
    virtual void Render() = 0;
};

/// <summary>
/// UpdateManagerからGarbageCollectの通知を受け取るInterfaceです。
/// </summary>
class IGarbageCollectReceiver : public IOrderable
{
public:
    /// <summary>
    /// フレームの最後、Objectの破棄処理の後に呼ばれます。
    /// </summary>
    virtual void OnGarbageCollect() = 0;
};