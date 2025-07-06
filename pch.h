#pragma once

/* === begin C++ standard library === */
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <format>
#include <ranges>
#include <numbers>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <utility>
/* === end C++ standard library === */

/* === begin dear imgui === */
#include <imgui.h>
#include <imgui_stdlib.h>
/* === end dear imgui === */

/* === begin implot === */
#include <implot.h>
/* === end implot === */

/* === begin cereal === */
#include <cereal/cereal.hpp>

#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
/* === end cereal === */

/* === begin directxtk12 === */
#include <directxtk12/SimpleMath.h>
/* === end directxtk12 === */

/* === begin dxlib === */
#include <DxLib.h>
#include "DxLib/dxlib_converter.h"
#include "DxLib/dxlib_serializer.h"
/* === end dxlib === */

/* === begin crossguid === */
#include <crossguid/guid.hpp>
/* === end crossguid === */

/* === begin engine === */
#include "object.h"
#include "logger.h"
#include "Math/mathf.h"
/* === end engine === */

/* === begin engine.serializer === */
#include "guid_serializer.h"
#include "Math/math_serializer.h"
#include "Rendering/rendering_serializer.h"
/* === end engine.serialzier === */