#pragma once
#include "font_data.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

namespace engine
{
enum class CommandType
{
    Mesh,
    Text,
    ProceduralMesh
};

struct RenderCommand
{
    uint64_t priority;
    
    CommandType type;

    union
    {
        struct
        {
            Vector3 *pos;
            Shader *shader;
            Material *material;
            Mesh *mesh;
            int sub_mesh_index;
            uint32_t instance_count;
            D3D12_GPU_VIRTUAL_ADDRESS world_matrix_buffer_address;
            D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_buffer_handle;
        } mesh_data;

        struct
        {
            FontData *font_data;
            Vector2 *position;
            const char *string;
            Color *color;
        } text_data;

        struct
        {
            Shader *shader;
            Material *material;
            uint32_t vertex_count;
        } procedural_mesh_data;
    };
};
}