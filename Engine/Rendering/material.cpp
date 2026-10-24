#include "pch.h"

#include "material.h"
#include "gui.h"
#include "Asset/asset_database.h"
#include "crs_bridge.h"
#include "texture2d.h"
#include "texture_cube.h"
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

/// <summary>
/// C++の型に対応する、Shaderの変数の型です。
/// </summary>
template <typename T>
constexpr kConstantBufferDataType DataTypeOf()
{
    if constexpr (std::is_same_v<T, int>)
        return kConstantBufferDataType::kConstantBufferDataTypeInt;
    else if constexpr (std::is_same_v<T, float>)
        return kConstantBufferDataType::kConstantBufferDataType_Float;
    else if constexpr (std::is_same_v<T, Vector2>)
        return kConstantBufferDataType::kConstantBufferDataType_Vector2;
    else if constexpr (std::is_same_v<T, Vector3>)
        return kConstantBufferDataType::kConstantBufferDataType_Vector3;
    else if constexpr (std::is_same_v<T, Color>)
        return kConstantBufferDataType::kConstantBufferDataType_Color;
    else if constexpr (std::is_same_v<T, Matrix>)
        return kConstantBufferDataType::kConstantBufferDataType_Matrix;
    else
        static_assert(sizeof(T) == 0, "Unsupported material value type");
}

// SetGlobal〜で設定された値の書き込み方。後から作ったMaterialにも書き込むために、名前ごとに覚えておく
std::unordered_map<std::string, std::function<void(Material &)>> g_global_writers;

/// <summary>
/// 今あるすべてのMaterialに対してactionを呼びます。
/// </summary>
template <typename Func>
void ForEachMaterial(Func &&action)
{
    for (const auto &object : Object::AllObjects() | std::views::values)
    {
        if (const auto material = std::dynamic_pointer_cast<Material>(object))
            action(*material);
    }
}
}

void Material::ApplyGlobals()
{
    for (const auto &writer : g_global_writers | std::views::values)
        writer(*this);
}

void Material::SetGlobalMaterialWriter(const std::string &name, std::function<void(Material &)> writer)
{
    ForEachMaterial([&](Material &material) {
        writer(material);
    });
    g_global_writers[name] = std::move(writer);
}

void Material::SetGlobalWriter(const std::string &name, std::function<void(MaterialBlock &)> writer)
{
    SetGlobalMaterialWriter(name, [writer = std::move(writer)](Material &material) {
        if (material.shared_material_block != nullptr)
            writer(*material.shared_material_block);
    });
}

void Material::SetGlobalBuffer(const std::string &name, const CRS::BufferHandle buffer)
{
    SetGlobalMaterialWriter(name, [name, buffer](Material &material) {
        // バッファを持たないMaterialには何もしない（警告も出さない）
        if (material.shared_material_block == nullptr ||
            (material.shared_material_block->GetStructuredBufferData(name) == nullptr &&
             material.shared_material_block->GetByteAddressBufferData(name) == nullptr))
            return;

        material.SetBufferOverride(name, buffer);
    });
}

bool Material::SetBuffer(const std::string &name, const CRS::BufferHandle buffer)
{
    if (shared_material_block == nullptr)
        CreateMaterialBlock();

    if (shared_material_block == nullptr || (shared_material_block->GetStructuredBufferData(name) == nullptr &&
                                             shared_material_block->GetByteAddressBufferData(name) == nullptr))
    {
        Logger::Warn<Material>("Material '%s' has no buffer '%s'", Name().c_str(), name.c_str());
        return false;
    }

    SetBufferOverride(name, buffer);
    return true;
}

void Material::SetBufferOverride(const std::string &name, const CRS::BufferHandle buffer)
{
    if (buffer.IsValid())
        m_buffer_overrides_[name] = buffer;
    else
        m_buffer_overrides_.erase(name);
    m_renderer_dirty_ = true;
}

template <typename T>
void Material::SetGlobalValue(const std::string &name, const T &value)
{
    // 同じ名前・型の変数があれば書き込む。無ければ何もしない（警告も出さない）
    SetGlobalWriter(name, [name, value](MaterialBlock &block) {
        const auto constant_buffer = block.FindConstantBufferData(name);
        if (constant_buffer != nullptr && constant_buffer->VariableType(name) == DataTypeOf<T>())
            constant_buffer->SetData(name, &value, sizeof(T));
    });
}

void Material::SetGlobalInt(const std::string &name, const int value)
{
    SetGlobalValue(name, value);
}

void Material::SetGlobalFloat(const std::string &name, const float value)
{
    SetGlobalValue(name, value);
}

void Material::SetGlobalVector2(const std::string &name, const Vector2 &value)
{
    SetGlobalValue(name, value);
}

void Material::SetGlobalVector3(const std::string &name, const Vector3 &value)
{
    SetGlobalValue(name, value);
}

void Material::SetGlobalColor(const std::string &name, const Color &value)
{
    SetGlobalValue(name, value);
}

void Material::SetGlobalMatrix(const std::string &name, const Matrix &value)
{
    SetGlobalValue(name, value);
}

void Material::SetGlobalTexture(const std::string &name, const AssetPtr<Texture2D> &texture)
{
    SetGlobalWriter(name, [name, texture](MaterialBlock &block) {
        if (const auto data = block.GetTextureBufferData(name))
            data->SetTexture(texture);
    });
}

void Material::SetGlobalTextureCube(const std::string &name, const AssetPtr<TextureCube> &texture)
{
    SetGlobalWriter(name, [name, texture](MaterialBlock &block) {
        if (const auto data = block.GetTextureCubeData(name))
            data->SetTexture(texture);
    });
}

void Material::SetGlobalBuffer(const std::string &name, const void *data, const size_t stride, const size_t count)
{
    const auto bytes = static_cast<const uint8_t *>(data);
    std::vector<uint8_t> copy(bytes, bytes + stride * count);
    SetGlobalWriter(name, [name, copy = std::move(copy), stride, count](MaterialBlock &block) {
        const auto buffer = block.GetStructuredBufferData(name);
        if (buffer == nullptr || buffer->Stride() != stride)
            return;

        buffer->SetCount(count);
        buffer->SetData(copy.data());
    });
}

void Material::SetGlobalRawBuffer(const std::string &name, const void *data, const size_t size)
{
    const auto bytes = static_cast<const uint8_t *>(data);
    std::vector<uint8_t> copy(bytes, bytes + size);
    SetGlobalWriter(name, [name, copy = std::move(copy)](MaterialBlock &block) {
        const auto buffer = block.GetByteAddressBufferData(name);
        if (buffer == nullptr)
            return;

        buffer->SetCount(copy.size());
        buffer->SetData(copy.data());
    });
}

void Material::SetGlobalSampler(const std::string &name, const CRS::Filter filter, const CRS::AddressMode address)
{
    SetGlobalWriter(name, [name, filter, address](MaterialBlock &block) {
        if (const auto sampler = block.GetSamplerData(name))
            sampler->SetState(static_cast<int>(filter), static_cast<int>(address));
    });
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

    if (constant_buffer->VariableType(name) != DataTypeOf<T>() || !constant_buffer->SetData(name, &value, sizeof(T)))
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

AssetPtr<TextureCube> Material::GetTextureCube(const std::string &name) const
{
    const auto texture_data = shared_material_block ? shared_material_block->GetTextureCubeData(name) : nullptr;
    return texture_data ? texture_data->Data() : AssetPtr<TextureCube>{};
}

bool Material::SetTextureCube(const std::string &name, const AssetPtr<TextureCube> &texture)
{
    if (shared_material_block == nullptr)
        CreateMaterialBlock();

    const auto texture_data = shared_material_block ? shared_material_block->GetTextureCubeData(name) : nullptr;
    if (texture_data == nullptr)
    {
        Logger::Warn<Material>("Material '%s' has no TextureCube '%s'", Name().c_str(), name.c_str());
        return false;
    }

    texture_data->SetTexture(texture);
    return true;
}

bool Material::SetBuffer(const std::string &name, const void *data, const size_t stride, const size_t count)
{
    if (shared_material_block == nullptr)
        CreateMaterialBlock();

    const auto buffer = shared_material_block ? shared_material_block->GetStructuredBufferData(name) : nullptr;
    if (buffer == nullptr)
    {
        Logger::Warn<Material>("Material '%s' has no StructuredBuffer '%s'", Name().c_str(), name.c_str());
        return false;
    }

    if (buffer->Stride() != stride)
    {
        Logger::Warn<Material>("StructuredBuffer '%s' expects %zu bytes per element, but %zu was given", name.c_str(),
                               buffer->Stride(), stride);
        return false;
    }

    buffer->SetCount(count);
    buffer->SetData(data);
    return true;
}

bool Material::SetRawBuffer(const std::string &name, const void *data, const size_t size)
{
    if (shared_material_block == nullptr)
        CreateMaterialBlock();

    const auto buffer = shared_material_block ? shared_material_block->GetByteAddressBufferData(name) : nullptr;
    if (buffer == nullptr)
    {
        Logger::Warn<Material>("Material '%s' has no ByteAddressBuffer '%s'", Name().c_str(), name.c_str());
        return false;
    }

    buffer->SetCount(size);
    buffer->SetData(data);
    return true;
}

bool Material::SetSampler(const std::string &name, const CRS::Filter filter, const CRS::AddressMode address)
{
    if (shared_material_block == nullptr)
        CreateMaterialBlock();

    const auto sampler = shared_material_block ? shared_material_block->GetSamplerData(name) : nullptr;
    if (sampler == nullptr)
    {
        Logger::Warn<Material>("Material '%s' has no sampler '%s'", Name().c_str(), name.c_str());
        return false;
    }

    sampler->SetState(static_cast<int>(filter), static_cast<int>(address));
    return true;
}

bool Material::HasProperty(const std::string &name) const
{
    if (shared_material_block == nullptr)
        return false;

    auto &block = *shared_material_block;
    return block.FindConstantBufferData(name) != nullptr || block.GetTextureBufferData(name) != nullptr ||
           block.GetTextureCubeData(name) != nullptr || block.GetStructuredBufferData(name) != nullptr ||
           block.GetByteAddressBufferData(name) != nullptr || block.GetSamplerData(name) != nullptr;
}

CRS::BufferHandle Material::UploadBuffer(const std::string &name, const void *data, const size_t size,
                                         const size_t stride, const bool raw)
{
    if (size == 0)
    {
        m_gpu_buffers_.erase(name);
        return {};
    }

    // ByteAddressBufferは4バイト単位で読むので、大きさを4の倍数に切り上げる
    const auto gpu_size = raw ? (size + 3) / 4 * 4 : size;
    auto &entry = m_gpu_buffers_[name];
    if (!entry.buffer.IsValid() || entry.size != gpu_size)
    {
        CRS::BufferDesc desc;
        desc.size = gpu_size;
        desc.stride = raw ? 0 : static_cast<uint32_t>(stride);
        desc.usage = raw ? CRS::BufferUsage::Raw : CRS::BufferUsage::Structured;
        desc.memory = CRS::MemoryType::Upload;
        desc.debug_name = Name() + " " + name;
        entry.buffer = CRS::Buffer(CrsBridge::Crs().Device(), desc);
        entry.size = gpu_size;
    }

    entry.buffer.Update(data, size);
    return entry.buffer.GetHandle();
}

CRS::SamplerHandle Material::GetSampler(const std::string &name, const int filter, const int address)
{
    auto &entry = m_gpu_samplers_[name];
    if (!entry.sampler.IsValid() || entry.filter != filter || entry.address != address)
    {
        CRS::SamplerDesc desc;
        desc.filter = static_cast<CRS::Filter>(filter);
        desc.address = static_cast<CRS::AddressMode>(address);
        entry.sampler = CRS::Sampler(CrsBridge::Crs().Device(), desc);
        entry.filter = filter;
        entry.address = address;
    }
    return entry.sampler.GetHandle();
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

        // 読み込んだMaterialや、Shaderが変わったMaterialにもグローバルな値を入れる
        ApplyGlobals();
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
            // RenderTexture / DepthTextureもTexture2Dの派生なので、ここでGPU上の描画先を渡す
            const auto texture = static_cast<TextureBufferData &>(data).Data().CastedLock();
            material->SetTexture(name, texture ? texture->GpuTexture() : CRS::TextureHandle{});
            break;
        }
        case kBufferType_TextureCube: {
            const auto texture = static_cast<TextureCubeBufferData &>(data).Data().CastedLock();
            material->SetTexture(name, texture ? texture->GpuTexture() : CRS::TextureHandle{});
            break;
        }
        case kBufferType_StructuredBuffer: {
            // 外から渡されたバッファがあれば、値のコピーではなくそのバッファを使う
            if (const auto itr = m_buffer_overrides_.find(name);
                itr != m_buffer_overrides_.end())
            {
                material->SetBuffer(name, itr->second);
                break;
            }
            auto &buffer = static_cast<StructuredBufferData &>(data);
            material->SetBuffer(name, UploadBuffer(name, buffer.Data(), buffer.Size(), buffer.Stride(), false));
            break;
        }
        case kBufferType_ByteAddressBuffer: {
            if (const auto itr = m_buffer_overrides_.find(name);
                itr != m_buffer_overrides_.end())
            {
                material->SetBuffer(name, itr->second);
                break;
            }
            auto &buffer = static_cast<ByteAddressBufferData &>(data);
            material->SetBuffer(name, UploadBuffer(name, buffer.Data(), buffer.Count(), 0, true));
            break;
        }
        case kBufferType_Sampler: {
            const auto &sampler = static_cast<SamplerData &>(data);
            material->SetSampler(name, GetSampler(name, sampler.Filter(), sampler.Address()));
            break;
        }
        default:
            // RWTextureなどのUAVは、Materialからは設定できない
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

    ApplyGlobals();
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
