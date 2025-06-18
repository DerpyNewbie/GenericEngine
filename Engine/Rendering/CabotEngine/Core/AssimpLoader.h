#pragma once
#define NOMINMAX
#include <memory>
#include <vector>
#include <assimp/mesh.h>
#include <assimp/scene.h>

#include "../Graphics/CabotMaterial.h"
#include "../Graphics/SharedStruct.h"

struct aiNode;
struct BoneNode;
struct CabotMesh;
struct Vertex;
struct aiMesh;
struct aiMaterial;

struct ModelResource
{
    const wchar_t *FileName = nullptr;
    std::vector<CabotMesh> &Meshes;
    CabotBone &Bone;
    std::vector<CabotMaterial> &Materials;
    bool inverseU;
    bool inverseV;
};

struct AnimResource
{
    const wchar_t *FileName = nullptr;
    AnimationData &Animation;
};

class AssimpLoader
{
public:
    bool ImportModel(ModelResource settings);
    bool ImportAnim(AnimResource settings);

private:
    void LoadMeshAndBone(std::vector<aiBone> &dst_bone, CabotMesh &dst_mesh, const aiMesh *src, bool inverseU,
                         bool inverseV);
    void LoadNode(aiNode &dst, aiNode &src, aiNode &parent);
    void LoadAnim(AnimationData &dst, aiAnimation *src);
};