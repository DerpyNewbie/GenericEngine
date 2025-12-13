#include "pch.h"
#include "trs.h"

namespace engine
{
TRS::TRS(Matrix matrix)
{
    matrix.Decompose(scale, rotation, translation);
}

Matrix TRS::GetMatrix() const
{
    return Matrix::CreateScale(scale) *
           Matrix::CreateFromQuaternion(rotation) *
           Matrix::CreateTranslation(translation);
}

TRS TRS::Blend(const TRS &from, const TRS &to, const float t)
{
    TRS final_trs = {};
    final_trs.translation += Mathf::Lerp(from.translation, to.translation, t);
    final_trs.scale = Mathf::Lerp(from.scale, to.scale, t);
    final_trs.rotation = Mathf::Slerp(final_trs.rotation, to.rotation, t);

    return final_trs;
}
}