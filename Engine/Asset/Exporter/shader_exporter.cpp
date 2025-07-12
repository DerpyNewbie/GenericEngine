#include "pch.h"
#include "shader_exporter.h"

#include "Asset/asset_descriptor.h"
#include "Rendering/shader.h"

namespace engine
{
std::vector<std::string> ShaderExporter::SupportedExtensions()
{
    return {".hlsl"};
}

void ShaderExporter::Export(AssetDescriptor *asset)
{
    const auto shader = std::dynamic_pointer_cast<Shader>(asset->managed_object);

    using namespace rapidjson;
    auto create_shader_param_obj = [](const std::shared_ptr<ShaderParameter> &param, auto &alloc) {
        auto result = Value(kObjectType);

        auto name_value = Value{};
        name_value.SetString(param->name.c_str(), param->name.size());
        result.AddMember("name", name_value, alloc);

        auto type_value = Value{};
        type_value.SetString(param->type_hint.c_str(), param->type_hint.size());
        result.AddMember("type", type_value, alloc);

        if (!param->display_name.empty())
        {
            auto display_name_value = Value{};
            display_name_value.SetString(param->display_name.c_str(), param->display_name.size());
            result.AddMember("displayName", display_name_value, alloc);
        }
        return result;
    };

    auto create_shader_params_list = [&](auto &params, auto &alloc) {
        auto result = Value(kArrayType);
        for (const auto &param : params)
        {
            result.PushBack(create_shader_param_obj(param, alloc), alloc);
        }
        return result;
    };

    auto create_shader_params_value = [&](auto &alloc) {
        auto vertex_params = std::views::filter(shader->parameters, [](auto &p) {
            return p->shader_type == kShaderType_Vertex;
        });
        auto pixel_params = std::views::filter(shader->parameters, [](auto &p) {
            return p->shader_type == kShaderType_Pixel;
        });

        auto vertex_params_obj = create_shader_params_list(vertex_params, alloc);
        auto pixel_params_obj = create_shader_params_list(pixel_params, alloc);

        Value result(kObjectType);
        result.AddMember("vertex", vertex_params_obj, alloc);
        result.AddMember("pixel", pixel_params_obj, alloc);
        return result;
    };

    Document doc;
    doc.SetObject();
    doc.AddMember("parameters", create_shader_params_value(doc.GetAllocator()), doc.GetAllocator());

    StringBuffer string_buffer;
    Writer writer(string_buffer);
    doc.Accept(writer);

    asset->ClearKeys();
    asset->SetString("shader_meta", string_buffer.GetString());
    //test
}
}