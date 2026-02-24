#pragma once
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "RenderEngine.h"
#include "VertexBuffer.h"

class Billboard
{
public:
    static Matrix CalcMatrix(Matrix &mat);
};