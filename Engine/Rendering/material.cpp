#include "pch.h"

#include "material.h"
#include "gui.h"
#include "Asset/asset_database.h"
#include "crs_bridge.h"
#include "texture2d.h"
#include "CaboRenderingSystem/render_engine.h"

namespace engine
{
namespace
{
constexpr auto kDefaultShaderPath = "Lit.hlsl";

CRS::RenderState ToRenderState(const ShaderSettings &settings)
{
    constexpr CRS::CompareOp kCompareOps[] = {
        CRS::CompareOp::Less, CRS::CompareOp::Greater, CRS::CompareOp::LessEqual, CRS::CompareOp::GreaterEqual,
        CRS::CompareOp::Equal, CRS::CompareOp::NotEqual, CRS::CompareOp::Always, CRS::CompareOp::Never
    };
    constexpr CRS::CullMode kCullModes[] = {CRS::CullMode::Back, CRS::CullMode::Front, CRS::CullMode::None};
    constexpr CRS::BlendMode kBlendModes[] = {
        CRS::BlendMode::Opaque, CRS::BlendMode::AlphaBlend, CRS::BlendMode::Additive
    };

    CRS::RenderState state;
    state.blend = kBlendModes[std::clamp(settings.blend_mode, 0, 2)];
    state.cull = kCullModes[std::clamp(settings.cull, 0, 2)];
    state.depth.compare = kCompareOps[std::clamp(settings.z_test, 0, 7)];
    state.depth.write_enable = settings.z_write == 0;
    return state;
}

/// <summary>
/// 基本色(_Color)の変数かどうかです。新しく作ったMaterialでは白にしておきます（0だと真っ黒になるため）。
/// </summary>
bool IsColorVariable(const std::string &name, const size_t size)
{
    return size == sizeof(Color) && name == "_Color";
}
}

template <typename T>
bool Material::SetValue(const std::string &name, const T &value)
{
    if (shared_material_block == nullptr)
        CreateMaterialBlock();
    if (shared_material_block == nullptr)
        return false;

    const auto constant_buffer = shared_material_block->FindConstantBufferData(name);
    if (constant_buffer == nullptr)
    {
        Logger::Warn<Material>("Material '%s' has no property '%s'", Name().c_str(), name.c_str());
        return false;
    }

    if (!constant_buffer->SetData(name, &value, sizeof(T)))
    {
        Logger::Warn<Material>("Material property '%s' has a different type", name.c_str());
        return false;
    }
    return true;
}

template <typename T>
T Material::GetValue(const std::string &name, const T &default_value) const
{
    if (shared_material_block == nullptr)
        return default_value;

    const auto constant_buffer = shared_material_block->FindConstantBufferData(name);
    T value;
    return constant_buffer != nullptr && constant_buffer->GetData(name, &value, sizeof(T)) ? value : default_value;
}

Material::~Material()
{
    if (m_renderer_material_.IsValid())
        CrsBridge::Crs().Materials().Remove(m_renderer_material_);
}

std::shared_ptr<Material> Material::Create(const std::string &shader_path, const std::string &name)
{
    const auto material = Instantiate<Material>(name);
    material->SetShader(AssetDatabase::GetAsset<Shader>(shader_path));
    return material;
}

bool Material::SetInt(const std::string &name, const int value)
{
    return SetValue(name, value);
}

bool Material::SetFloat(const std::string &name, const float value)
{
    return SetValue(name, value);
}

bool Material::SetVector2(const std::string &name, const Vector2 &value)
{
    return SetValue(name, value);
}

bool Material::SetVector3(const std::string &name, const Vector3 &value)
{
    return SetValue(name, value);
}

bool Material::SetColor(const std::string &name, const Color &value)
{
    return SetValue(name, value);
}

bool Material::SetMatrix(const std::string &name, const Matrix &value)
{
    return SetValue(name, value);
}

bool Material::SetTexture(const std::string &name, const AssetPtr<Texture2D> &texture)
{
    if (shared_material_block == nullptr)
        CreateMaterialBlock();

    const auto texture_data = shared_material_block ? shared_material_block->GetTextureBufferData(name) : nullptr;
    if (texture_data == nullptr)
    {
        Logger::Warn<Material>("Material '%s' has no texture '%s'", Name().c_str(), name.c_str());
        return false;
    }

    texture_data->SetTexture(texture);
    return true;
}

int Material::GetInt(const std::string &name, const int default_value) const
{
    return GetValue(name, default_value);
}

float Material::GetFloat(const std::string &name, const float default_value) const
{
    return GetValue(name, default_value);
}

Vector2 Material::GetVector2(const std::string &name, const Vector2 &default_value) const
{
    return GetValue(name, default_value);
}

Vector3 Material::GetVector3(const std::string &name, const Vector3 &default_value) const
{
    return GetValue(name, default_value);
}

Color Material::GetColor(const std::string &name, const Color &default_value) const
{
    return GetValue(name, default_value);
}

Matrix Material::GetMatrix(const std::string &name, const Matrix &default_value) const
{
    return GetValue(name, default_value);
}

AssetPtr<Texture2D> Material::GetTexture(const std::string &name) const
{
    const auto texture_data = shared_material_block ? shared_material_block->GetTextureBufferData(name) : nullptr;
    return texture_data ? texture_data->Data() : AssetPtr<Texture2D>{};
}

bool Material::HasProperty(const std::string &name) const
{
    return shared_material_block != nullptr && (shared_material_block->FindConstantBufferData(name) != nullptr ||
                                                shared_material_block->GetTextureBufferData(name) != nullptr);
}

CRS::MaterialId Material::GetRendererMaterial()
{
    auto &crs = CrsBridge::Crs();
    const auto shader = m_shader_.CastedLock();

    if (crs.Materials().Get(m_renderer_material_) == nullptr || m_renderer_shader_ != shader.get())
    {
        CreateRendererMaterial(shader);

        // 保存されていたMaterialBlockに、Shaderにあって足りないパラメーターを足す（設定済みの値は残す）
        if (shared_material_block != nullptr && shader != nullptr)
        {
            const bool had_color = shared_material_block->FindConstantBufferData("_Color") != nullptr;
            shared_material_block->LoadShaderParameters(shader->parameters);
            if (!had_color && shared_material_block->FindConstantBufferData("_Color") != nullptr)
                SetColor("_Color", Color(1, 1, 1, 1));
        }
    }

    if (shared_material_block == nullptr)
        CreateMaterialBlock();

    const bool block_changed = shared_material_block != nullptr && shared_material_block->ConsumeDirty();
    if (m_renderer_dirty_ || block_changed)
    {
        SyncRendererMaterial(shader);
        m_renderer_dirty_ = false;
    }

    return m_renderer_material_;
}

void Material::CreateRendererMaterial(const std::shared_ptr<Shader> &shader)
{
    auto &crs = CrsBridge::Crs();
    if (m_renderer_material_.IsValid())
        crs.Materials().Remove(m_renderer_material_);

    auto material = crs.Pipeline().CreateDefaultMaterial();
    if (const auto program = shader ? shader->CrsProgram() : nullptr)
    {
        CRS::MaterialPass forward;
        forward.vertex_shader = program->vertex.GetHandle();
        forward.pixel_shader = program->pixel.GetHandle();
        forward.render_state = ToRenderState(shader->ShaderSettings());
        material.SetPass(CRS::PassTag::Forward, forward);

        // 影用の入口(VSShadow)が無いShaderは、標準の影を使う。半透明は影を落とさない
        if (program->shadow_vertex.IsValid())
        {
            CRS::MaterialPass shadow;
            shadow.vertex_shader = program->shadow_vertex.GetHandle();
            shadow.render_state.cull = CRS::CullMode::None;
            material.SetPass(CRS::PassTag::Shadow, shadow);
        }
        else if (forward.render_state.blend != CRS::BlendMode::Opaque)
        {
            material.RemovePass(CRS::PassTag::Shadow);
        }

        material.BuildPropertyTable(crs.Device());
    }

    m_renderer_material_ = crs.Materials().Add(std::move(material));
    m_renderer_shader_ = shader.get();
    m_renderer_dirty_ = true;
}

void Material::SyncRendererMaterial(const std::shared_ptr<Shader> &shader)
{
    const auto material = CrsBridge::Crs().Materials().Get(m_renderer_material_);
    if (material == nullptr)
        return;

    material->render_queue = render_queue;

    if (shader != nullptr)
    {
        if (const auto forward = material->GetPass(CRS::PassTag::Forward))
        {
            auto pass = *forward;
            pass.render_state = ToRenderState(shader->ShaderSettings());
            if (!(pass.render_state == forward->render_state))
                material->SetPass(CRS::PassTag::Forward, pass);
        }
    }

    if (shared_material_block == nullptr)
        return;

    // MaterialBlockの値を、Shaderの変数名でそのまま渡す
    shared_material_block->ForEachBufferData([&](const std::string &name, BufferDataBase &data) {
        switch (data.BufferType())
        {
        case kBufferType_ConstantBuffer:
            static_cast<ConstantBufferData &>(data).ForEachVariable(
                [&](const std::string &variable, const void *value, const size_t size) {
                    material->SetData(variable, value, static_cast<uint32_t>(size));
                });
            break;
        case kBufferType_Texture2D: {
            const auto texture = static_cast<TextureBufferData &>(data).Data().CastedLock();
            material->SetTexture(name, texture ? texture->GpuTexture() : CRS::TextureHandle{});
            break;
        }
        default:
            break;
        }
    });
}

void Material::OnInspectorGui()
{
    // Inspectorで表示している間は、変更を毎フレーム反映する
    m_renderer_dirty_ = true;

    int render_queue_int = render_queue;
    if (Gui::PropertyField("RenderQueue", render_queue_int))
    {
        render_queue = static_cast<uint16_t>(std::clamp(render_queue_int, 0, 10000));
    }

    if (Gui::ExpandablePropertyField<Shader>("shader", m_shader_))
    {
        if (m_shader_ != nullptr)
        {
            CreateMaterialBlock();
            return;
        }
    }

    if (shared_material_block == nullptr)
    {
        ImGui::Text("Material Block is not created.");
    }
    else
    {
        shared_material_block->OnInspectorGui();
    }
}

void Material::OnConstructed()
{
    m_shader_ = AssetDatabase::GetAsset<Shader>(kDefaultShaderPath);
    if (m_shader_.Lock() == nullptr)
    {
        Logger::Warn<Material>("Failed to find Engine Assets");
    }

    CreateMaterialBlock();
}

void Material::CreateMaterialBlock()
{
    if (m_shader_.CastedLock() == nullptr)
    {
        Logger::Error<Material>("Shader is null. Cannot create MaterialBlock.");
        return;
    }

    shared_material_block = Instantiate<MaterialBlock>("Material Block of " + Name());
    shared_material_block->LoadShaderParameters(m_shader_.CastedLock()->parameters);

    // _Colorは白から始める
    shared_material_block->ForEachBufferData([](const std::string &, BufferDataBase &data) {
        if (data.BufferType() != kBufferType_ConstantBuffer)
            return;

        auto &constant_buffer = static_cast<ConstantBufferData &>(data);
        std::vector<std::string> color_variables;
        constant_buffer.ForEachVariable([&](const std::string &variable, const void *, const size_t size) {
            if (IsColorVariable(variable, size))
                color_variables.push_back(variable);
        });

        const Color white(1, 1, 1, 1);
        for (const auto &variable : color_variables)
            constant_buffer.SetData(variable, &white, sizeof(white));
    });

    m_renderer_dirty_ = true;
}

AssetPtr<Shader> Material::GetShader()
{
    return m_shader_;
}

void Material::SetShader(const AssetPtr<Shader> &shader)
{
    m_shader_ = shader;
    CreateMaterialBlock();
}
}

CEREAL_REGISTER_TYPE(engine::Material)
