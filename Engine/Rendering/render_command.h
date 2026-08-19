#pragma once
#include <variant>
#include "font_data.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

namespace engine
{
struct MeshCommand
{
    Vector3 pos;
    Shader *shader;
    Material *material;
    Mesh *mesh;
    int sub_mesh_index;
    uint32_t instance_count;
    D3D12_GPU_VIRTUAL_ADDRESS world_matrix_buffer_address;
    D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_buffer_handle;
};

struct TextCommand
{
    FontData *font_data;
    Vector2 position;
    const char *string;
    Color color;
    float rotation;
    Vector2 origin;
    float scale;
    uint16_t render_queue;
};

struct ProceduralCommand
{
    Shader *shader;
    Material *material;
    size_t vertex_count;
};

struct EffectCommand
{
    uint16_t render_queue;
};

struct RenderCommand
{
    uint64_t priority;
    UINT64 target_camera_id;
    std::variant<MeshCommand, TextCommand, ProceduralCommand, EffectCommand> data;
};
}