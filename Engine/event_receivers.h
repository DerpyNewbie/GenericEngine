#pragma once

class IOrderable
{
public:
    virtual ~IOrderable() = default;
    virtual int Order()
    {
        return 0;
    }
};

class IUpdateReceiver : public IOrderable
{
public:
    virtual void OnUpdate() = 0;
};

class IFixedUpdateReceiver : public IOrderable
{
public:
    virtual void OnFixedUpdate() = 0;
};