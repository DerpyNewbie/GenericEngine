#pragma once

#include <fstream>
#include <memory>
#include <filesystem>
#include <chrono>
#include <string>
#include <format>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <thread>
#include <utility>

#include <windows.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <implot.h>

#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

#include "Math/math_serializer.h"