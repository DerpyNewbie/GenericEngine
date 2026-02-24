#pragma once
#include "mesh.h"

namespace engine
{
class Primitives
{
    inline static std::shared_ptr<Mesh> m_quad_mesh_;
    
public:
    static std::shared_ptr<Mesh> GetQuadMesh();
    
};
}