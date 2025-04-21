#pragma once

class IUpdateReceiver
{
public:
    virtual ~IUpdateReceiver() = default;
    virtual void OnUpdate() = 0;
};

class IFixedUpdateReceiver
{
    public:
    virtual ~IFixedUpdateReceiver() = default;
    virtual void OnFixedUpdate() = 0;
};

class IDrawCallReceiver
{
    public:
    virtual ~IDrawCallReceiver() = default;
    virtual void OnDraw() = 0;
};
