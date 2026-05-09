#pragma once

template <typename T>
concept kAllowedBufferType =
    std::same_as<T, int> ||
    std::same_as<T, float> ||
    std::same_as<T, Color> ||
    std::same_as<T, Vector2> ||
    std::same_as<T, Vector3>;