#pragma once

namespace engine
{
struct YieldBase
{
    virtual bool should_resume() = 0;
    virtual ~YieldBase() = default;
};
}