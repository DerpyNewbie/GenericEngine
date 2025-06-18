#pragma once
#include <DirectXMath.h>
#include <assimp/matrix4x4.h>

inline DirectX::XMMATRIX aiMatrixToXMMatrix(const aiMatrix4x4t<float>& src)
{
    return DirectX::XMMATRIX({src.a1, src.b1, src.c1, src.d1},
                             {src.a2, src.b2, src.c2, src.d2},
                             {src.a3, src.b3, src.c3, src.d3},
                             {src.a4, src.b4, src.c4, src.d4});
}

inline aiMatrix4x4 XMMatrixToaiMatrix(const DirectX::XMMATRIX src)
{
    return aiMatrix4x4(src.r->m128_f32[0], src.r->m128_f32[4], src.r->m128_f32[8], src.r->m128_f32[12],
                       src.r->m128_f32[1], src.r->m128_f32[5], src.r->m128_f32[9], src.r->m128_f32[13],
                       src.r->m128_f32[2], src.r->m128_f32[6], src.r->m128_f32[10], src.r->m128_f32[14],
                       src.r->m128_f32[3], src.r->m128_f32[7], src.r->m128_f32[11], src.r->m128_f32[15]
    );
}
