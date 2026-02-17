#pragma once
#include "mesh.h"

class Primitives
{
    inline static std::shared_ptr<engine::Mesh> m_quad_mesh_;
    
public:
    static std::shared_ptr<engine::Mesh> GetQuadMesh();
    
};
