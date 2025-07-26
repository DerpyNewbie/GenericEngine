#pragma once

/* === begin C++ standard library === */
#include <array>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
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
#include <queue>
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

/* === begin windows === */
#include <windows.h>
#include <wrl.h>
#include <wincodec.h>
/* === end windows === */

/* === begin directx === */
#include <directx/d3dx12.h>
#include <directx/d3dcommon.h>
#include <DirectXTex.h>
#include <directxtk12/SimpleMath.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
/* === end directx === */

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

/* === begin engine.cabot_engine === */
#include "Rendering/CabotEngine/Graphics/ComPtr.h"
/* === end engine.cabot_engine === */