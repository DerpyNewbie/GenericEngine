#pragma once
#include <directxtk12/SimpleMath.h>
#include <ThirdParty/Effekseer/include/Effekseer/Effekseer.h>

namespace EffekseerUtil
{
Effekseer::Matrix44 ToMatrix44(const DirectX::SimpleMath::Matrix &src);
DirectX::SimpleMath::Matrix ToMatrix(Effekseer::Matrix44 src);
}