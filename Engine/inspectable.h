#pragma once

namespace engine
{
class Inspectable
{
public:
    virtual ~Inspectable() = default;
    virtual void OnInspectorGui() = 0;
};
}