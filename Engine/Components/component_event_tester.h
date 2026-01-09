#pragma once
#include <cstdint>
#include <cereal/cereal.hpp>
#include <cereal/types/base_class.hpp>

#include "component.h"

namespace engine
{
class ComponentEventTester : public Component
{
    bool m_log_update_functions_ = false;

    void Log(const std::string &message) const;

public:
    #define ADD_LOGGING_FUNC(function) void function override {\
        Log(#function);\
        }

    ADD_LOGGING_FUNC(OnConstructed())
    ADD_LOGGING_FUNC(OnDeserialized())
    ADD_LOGGING_FUNC(OnValidate())
    ADD_LOGGING_FUNC(OnDestroy())
    ADD_LOGGING_FUNC(OnAwake())
    ADD_LOGGING_FUNC(OnStart())
    ADD_LOGGING_FUNC(OnEnabled())
    ADD_LOGGING_FUNC(OnDisabled())

    #undef ADD_LOGGING_FUNC

    void OnInspectorGui() override;
    void OnUpdate() override;
    void OnFixedUpdate() override;
    void OnRender() override;
    void OnCollisionEnter(const Collision &collision) override;
    void OnCollisionStay(const Collision &collision) override;
    void OnCollisionExit(const Collision &collision) override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Component>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::ComponentEventTester, 1)