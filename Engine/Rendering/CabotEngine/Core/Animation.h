#pragma once
#include "../Graphics/SharedStruct.h"

class Animation
{
public:
    Animation() = default;
    bool LoadAnim(std::wstring anim_path);

private:
    AnimationData m_AnimRootNode;
    
    friend class Animator;
};
