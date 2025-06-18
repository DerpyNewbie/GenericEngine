#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <assimp/scene.h>
#include "Animation.h"
#include "SkinnedModel.h"

class Animator
{
public:
    Animator(const std::shared_ptr<SkinnedModel>& model, const std::shared_ptr<Animation>& animation);
    
    void Update(float delta_time);
    
    void SetAnimation(const std::shared_ptr<Animation>& animation);

    static aiMatrix4x4 GetGlobalTransform(aiNode* node);

private:
    std::shared_ptr<SkinnedModel> m_Model;
    std::shared_ptr<Animation> m_Animation;
    std::unordered_map<std::string, aiNode*> m_ModelNodes;
    float m_CurrentTime;
    
    void GetBoneFinalTransform(std::vector<DirectX::XMMATRIX>& dst);
    void CreateMappedNode(aiNode* node);
    void UpdateNodeAnimation(float time, std::vector<std::shared_ptr<aiNodeAnim>> nodeAnims);
    aiMatrix4x4 CreateTransform(const aiVector3D& scaling, const aiQuaternion& rotation, const aiVector3D& position);
    aiVector3D CalcInterpolatedPosition(float animationTime, std::shared_ptr<aiNodeAnim> nodeAnim);
    aiQuaternion CalcInterpolatedRotation(float animationTime, std::shared_ptr<aiNodeAnim> nodeAnim);
    aiVector3D CalcInterpolatedScale(float animationTime, std::shared_ptr<aiNodeAnim> nodeAnim);
};
