#pragma once
#include "inspectable.h"
#include "object.h"

namespace engine
{
class InspectableAsset : public Object, public Inspectable
{};
}