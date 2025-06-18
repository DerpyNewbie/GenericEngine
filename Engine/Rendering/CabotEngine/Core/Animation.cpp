#include "Animation.h"

#include "AssimpLoader.h"

bool Animation::LoadAnim(std::wstring anim_path)
{
    AnimResource anim_resource = {
        anim_path.c_str(),
        m_AnimRootNode
    };

    AssimpLoader loader;
    if (!loader.ImportAnim(anim_resource))
    {
        return false;
    }
    
    return true;
}
