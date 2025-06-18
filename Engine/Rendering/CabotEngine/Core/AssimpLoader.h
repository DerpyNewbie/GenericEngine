#pragma once
#define NOMINMAX
#include <memory>
#include <vector>
#include <assimp/mesh.h>
#include <assimp/scene.h>

#include "../Graphics/Material.h"
#include "../Graphics/SharedStruct.h"

struct aiNode;
struct BoneNode;
struct Mesh;
struct Vertex;
struct aiMesh;
struct aiMaterial;

struct ModelResource
{
    const wchar_t* FileName = nullptr;
    std::vector<Mesh>& Meshes;
    Bone& Bone;
    std::vector<Material>& Materials;
    bool inverseU;
    bool inverseV;
};

struct AnimResource
{
    const wchar_t* FileName = nullptr;
    AnimationData& Animation;
};

class AssimpLoader
{
public:
    bool ImportModel(ModelResource settings);
    bool ImportAnim(AnimResource settings);
    
private:
    void LoadMeshAndBone(std::vector<aiBone>& dst_bone, Mesh& dst_mesh, const aiMesh* src, bool inverseU, bool inverseV);
    void LoadNode(aiNode& dst,aiNode& src,aiNode& parent);
    void LoadAnim(AnimationData& dst, aiAnimation* src);
};
