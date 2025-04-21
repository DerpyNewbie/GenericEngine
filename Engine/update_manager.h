#pragma once
#include "event_receivers.h"

#include <memory>
#include <vector>

class UpdateManager
{
    std::vector<std::shared_ptr<IUpdateReceiver>> m_update_receivers_;
public:

    // TODO: implement update manager that can be subscribed from anywhere
};
