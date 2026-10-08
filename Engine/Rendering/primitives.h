#pragma once
#include "mesh.h"

namespace engine
{
/// <summary>
/// Quadなどの基本的なMeshを提供するクラスです。
/// </summary>
class Primitives
{
    inline static std::shared_ptr<Mesh> m_quad_mesh_;
    
public:
    /// <summary>
    /// (-1, -1) から (1, 1) の四角形のMeshを取得します。初めて呼ばれた時に生成されます。
    /// </summary>
    static std::shared_ptr<Mesh> GetQuadMesh();
    
};
}