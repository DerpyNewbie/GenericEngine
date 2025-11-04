#include "pch.h"
#include "trs.h"

engine::TRS engine::TRS::BlendTRS(TRS rhs, TRS lhs, float t)
{
    TRS final_trs = {};
    final_trs.translate += rhs.translate * t;
    final_trs.scale += rhs.scale * t;
    final_trs.rotation = Mathf::Slerp(final_trs.rotation, rhs.rotation, t);

    t = 1.f - t;
    final_trs.translate += lhs.translate * t;
    final_trs.scale += lhs.scale * t;
    final_trs.rotation = Mathf::Slerp(final_trs.rotation, lhs.rotation, t);

    return final_trs;
}