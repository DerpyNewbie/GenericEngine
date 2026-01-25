#include "effekseer_util.h"

Effekseer::Matrix44 EffekseerUtil::ToMatrix44(const DirectX::SimpleMath::Matrix &src)
{
    Effekseer::Matrix44 effek_mat;
    memcpy(&effek_mat.Values, &src.m, sizeof(float) * 16);
    return effek_mat;
}
DirectX::SimpleMath::Matrix EffekseerUtil::ToMatrix(Effekseer::Matrix44 src)
{
    DirectX::SimpleMath::Matrix dx_mat;
    memcpy(&dx_mat, &src.Values, sizeof(float) * 16);
    return dx_mat;
}