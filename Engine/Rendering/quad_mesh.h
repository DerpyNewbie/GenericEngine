#pragma once
#include "mesh.h"

class QuadMesh
{
    inline static std::shared_ptr<engine::Mesh> m_mesh_;
    
public:

    static std::shared_ptr<engine::Mesh> GetMesh();
    
};
