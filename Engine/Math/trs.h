#pragma once

namespace engine
{
struct TRS
{
    Vector3 translate;
    Vector3 scale;
    Quaternion rotation;

    Matrix GetMatrix()
    {
        return Matrix::CreateScale(scale) *
               Matrix::CreateFromQuaternion(rotation) *
               Matrix::CreateTranslation(translate);
    }

    static TRS BlendTRS(TRS rhs, TRS lhs, float t);
};
}