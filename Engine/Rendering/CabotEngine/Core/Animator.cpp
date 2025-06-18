#include "Animator.h"
#include "../Converter/D3D12ToAssimp.h"
#include <DirectXMath.h>

Animator::Animator(const std::shared_ptr<SkinnedModel>& model, const std::shared_ptr<Animation>& animation):
    m_Model(model),
    m_Animation(animation),
    m_CurrentTime(0)
{
    CreateMappedNode(m_Model->m_Bone.RootNode);
}

void Animator::Update(float delta_time)
{
    auto& node_anims = m_Animation->m_AnimRootNode.Channels;

    //アニメーションの時間まで切り詰める
    m_CurrentTime += delta_time * m_Animation->m_AnimRootNode.TicksPerSecond;
    m_CurrentTime = fmod(m_CurrentTime, m_Animation->m_AnimRootNode.Duration);
    
    UpdateNodeAnimation(m_CurrentTime, node_anims);
    
    auto& armature = m_Model->m_Bone.Transforms;
    
    GetBoneFinalTransform(armature);
    m_Model->ApplyAnimation();
}

void Animator::SetAnimation(const std::shared_ptr<Animation>& animation)
{
    m_Animation = animation;
}

void Animator::UpdateNodeAnimation(float time, std::vector<std::shared_ptr<aiNodeAnim>> nodeAnims)
{
    for (auto node_anim : nodeAnims)
    {
        const auto node_handle = m_ModelNodes[node_anim->mNodeName.C_Str()];

        if (node_handle)
        {
            auto scale = CalcInterpolatedScale(time, node_anim);
            auto rotation = CalcInterpolatedRotation(time, node_anim);
            auto position = CalcInterpolatedPosition(time, node_anim);

            node_handle->mTransformation = CreateTransform(scale,rotation,position);
        }
    }
}

aiMatrix4x4 Animator::GetGlobalTransform(aiNode* node)
{
    aiMatrix4x4 transform = node->mTransformation;
    aiNode* parent = node->mParent;

    while (parent)
    {
        transform = parent->mTransformation * transform;
        parent = parent->mParent;
    }

    return transform;
}

aiMatrix4x4 Animator::CreateTransform(const aiVector3D& scaling, const aiQuaternion& rotation, const aiVector3D& position)
{
    aiMatrix4x4 scaleMatrix;
    aiMatrix4x4::Scaling(scaling, scaleMatrix);

    aiMatrix4x4 rotationMatrix = aiMatrix4x4(rotation.GetMatrix());

    aiMatrix4x4 translationMatrix;
    aiMatrix4x4::Translation(position, translationMatrix);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

void Animator::CreateMappedNode(aiNode* node)
{
    m_ModelNodes.emplace(node->mName.C_Str(),node);
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        CreateMappedNode(node->mChildren[i]);
    }
}

void Animator::GetBoneFinalTransform(std::vector<DirectX::XMMATRIX>& dst)
{
    auto& bones = m_Model->m_Bone.Offsets;
    for (int i = 0; i < bones.size(); ++i)
    {
        auto handle_node = m_ModelNodes[bones[i].mName.C_Str()];
        DirectX::XMMATRIX transform;

        if (handle_node == nullptr)
        {
            transform = aiMatrixToXMMatrix(GetGlobalTransform(handle_node));
        }
        else
        {
            transform = aiMatrixToXMMatrix(GetGlobalTransform(handle_node) * bones[i].mOffsetMatrix);
        }
        dst[i] = transform;
    }
}

aiVector3D Animator::CalcInterpolatedPosition(float animationTime, const std::shared_ptr<aiNodeAnim> nodeAnim)
{
    if (nodeAnim->mNumPositionKeys == 1)
    {
        return nodeAnim->mPositionKeys[0].mValue;
    }

    // 適切なキーフレームペアを探す
    unsigned int positionIndex = 0;
    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; ++i)
    {
        if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
        {
            positionIndex = i;
            break;
        }
    }

    unsigned int nextPositionIndex = positionIndex + 1;
    float t1 = (float)nodeAnim->mPositionKeys[positionIndex].mTime;
    float t2 = (float)nodeAnim->mPositionKeys[nextPositionIndex].mTime;
    float deltaTime = t2 - t1;

    if (deltaTime <= 0.0f)
        return nodeAnim->mPositionKeys[positionIndex].mValue;

    float factor = (animationTime - t1) / deltaTime;

    const aiVector3D& start = nodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIndex].mValue;

    // 線形補間 (LERP)
    return start + factor * (end - start);
}

aiQuaternion Animator::CalcInterpolatedRotation(float animationTime, const std::shared_ptr<aiNodeAnim> nodeAnim)
{
    if (nodeAnim->mNumRotationKeys == 1)
        return nodeAnim->mRotationKeys[0].mValue;

    unsigned int rotationIndex = 0;
    for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; ++i)
    {
        if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
        {
            rotationIndex = i;
            break;
        }
    }

    unsigned int nextRotationIndex = rotationIndex + 1;
    float t1 = (float)nodeAnim->mRotationKeys[rotationIndex].mTime;
    float t2 = (float)nodeAnim->mRotationKeys[nextRotationIndex].mTime;
    float deltaTime = t2 - t1;

    // 安全対策（ゼロ割防止）
    if (deltaTime <= 0.0f)
        return nodeAnim->mRotationKeys[rotationIndex].mValue;

    float factor = (animationTime - t1) / deltaTime;

    const aiQuaternion& startRotation = nodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& endRotation = nodeAnim->mRotationKeys[nextRotationIndex].mValue;

    aiQuaternion interpolated;
    aiQuaternion::Interpolate(interpolated, startRotation, endRotation, factor);
    interpolated.Normalize(); // 正規化して安全性アップ

    return interpolated;
}

aiVector3D Animator::CalcInterpolatedScale(float animationTime, const std::shared_ptr<aiNodeAnim> nodeAnim)
{
    if (nodeAnim->mNumScalingKeys == 1)
    {
        return nodeAnim->mScalingKeys[0].mValue;
    }

    // 適切なキーフレームペアを探す
    unsigned int ScalingIndex = 0;
    for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; ++i)
    {
        if (animationTime < static_cast<float>(nodeAnim->mScalingKeys[i + 1].mTime))
        {
            ScalingIndex = i;
            break;
        }
    }

    unsigned int nextScalingIndex = ScalingIndex + 1;
    float t1 = static_cast<float>(nodeAnim->mScalingKeys[ScalingIndex].mTime);
    float t2 = static_cast<float>(nodeAnim->mScalingKeys[nextScalingIndex].mTime);
    float deltaTime = t2 - t1;

    if (deltaTime <= 0.0f)
        return nodeAnim->mScalingKeys[ScalingIndex].mValue;

    float factor = (animationTime - t1) / deltaTime;

    const aiVector3D& start = nodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIndex].mValue;

    // 線形補間 (LERP)
    return start + factor * (end - start);
}