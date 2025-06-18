#pragma once
#include <directx/d3dx12.h>
#include <DirectXMath.h>
#include <assimp/anim.h>
#include <assimp/mesh.h>

#include "ComPtr.h"
#include "Texture2D.h"

struct CabotVertex
{
    DirectX::XMFLOAT3 Position; // 位置座標
    DirectX::XMFLOAT3 Normal; // 法線
    DirectX::XMFLOAT2 UV; // uv座標
    DirectX::XMFLOAT3 Tangent; // 接空間
    DirectX::XMFLOAT4 Color; // 頂点色
    unsigned int BoneIDs[4];
    float BoneWeights[4];
    unsigned int BoneNum = 0;

    static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
    static const int InputElementCount = 8;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct AnimationData
{
    double Duration;
    double TicksPerSecond;
    std::vector<std::shared_ptr<aiNodeAnim>> Channels;
};

struct CabotMesh
{
    static constexpr char BoneNumMax = 4;

    std::vector<DirectX::XMFLOAT3> Position;
    std::vector<DirectX::XMFLOAT3> Normal;
    std::vector<DirectX::XMFLOAT2> UV;
    std::vector<DirectX::XMFLOAT3> Tangent;
    std::vector<DirectX::XMFLOAT4> Color;
    std::vector<unsigned int> BoneIDs[4];
    std::vector<float> BoneWeights[4];
    std::vector<unsigned int> BoneNum = {0};
    std::vector<uint32_t> Indices; // インデックスの配列
};

struct CabotBone
{
    std::vector<aiBone> Offsets;
    std::vector<DirectX::XMMATRIX> Transforms;
    aiNode *RootNode;
};

struct alignas(256) Transform
{
    DirectX::XMMATRIX World; // ワールド行列
    DirectX::XMMATRIX View; // ビュー行列
    DirectX::XMMATRIX Proj; // 投影行列
};

struct SkinnedMeshData
{
    std::vector<std::shared_ptr<CabotMesh>> Meshes;
    std::vector<std::shared_ptr<aiBone>> Bones;
    std::shared_ptr<std::vector<DirectX::XMMATRIX>> Armature;
    //shared_ptrにすると都合が悪いのでraw_ptrで扱う;
    aiNode *RootNode;
};