#pragma once
#include "object.h"

namespace engine
{

/// <summary>
/// Shader object representation
/// </summary>
/// <remarks>
/// Base of all shaders used in the engine. Shaders such as PS, VS inherits from here.
/// </remarks>
class Shader : public Object
{
    // TODO: compiled shader object
};

class PixelShader : public Shader
{};

class VertexShader : public Shader
{};
}