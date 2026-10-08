#pragma once
#include "font_data.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

namespace engine
{
/// <summary>
/// RenderCommandの種類です。
/// </summary>
enum class CommandType
{
    Mesh,
    Effekseer,
    Text
};

/// <summary>
/// 描画1回分の情報をまとめたCommandです。priorityの順に描画されます。
/// </summary>
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
    };
};
}