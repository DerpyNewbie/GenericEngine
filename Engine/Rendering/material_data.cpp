#include "pch.h"
#include "material_data.h"

engine::MaterialData::MaterialData(ShaderParameter new_parameter): MaterialData({}, std::move(new_parameter))
{}

engine::MaterialData::MaterialData(const AssetPtr<IBuffer> &new_value, ShaderParameter new_parameter): parameter(std::move(new_parameter)), buffer(new_value)
{
    switch (new_value->BufferType())
    {
        case kParameterBufferType_ConstantBuffer:
            buffer_type = kGpuUploadType_CBV;
            break;
        case kParameterBufferType_StructuredBuffer:
            buffer_type = kGpuUploadType_SRV;
            break;
        case kParameterBufferType_Texture2D:
            buffer_type = kGpuUploadType_SRV;
            break;

        default: ;
    }
}

void engine::MaterialData::OnInspectorGui()
{
    auto name = parameter.display_name.empty() ? parameter.name.c_str() : parameter.display_name.c_str();
    ImGui::Text("Material Data: %s", name);
    ImGui::Text("Type: %s", parameter.type_hint.c_str());

    switch (buffer->BufferType())
    {
        case kParameterBufferType_ConstantBuffer:
            Gui::PropertyField<IConstantBufferAsset>("Buffer", buffer);
            break;
        case kParameterBufferType_StructuredBuffer:
            Gui::PropertyField<IStructuredBufferAsset>("Buffer", buffer);
            break;
        case kParameterBufferType_Texture2D:
            Gui::PropertyField<Texture2D>("Buffer", buffer);
            break;

        default: ;
    }

    if (buffer != nullptr)
    {
        buffer->OnInspectorGui();
        ImGui::Text("Buffer is valid: %s", buffer->IsValid() ? "true" : "false");
    }
    else
    {
        ImGui::Text("Buffer is null");
    }
}

void engine::MaterialData::SetValue(const AssetPtr<IBuffer> &new_buffer)
{
    buffer = new_buffer;
}

void engine::MaterialData::SetValue(const AssetPtr<Texture2D> &new_texture)
{
    buffer = AssetPtr<IBuffer>::FromInstance(new_texture.CastedLock());
}

bool engine::MaterialData::CreateBuffer() const
{
    if (buffer == nullptr)
    {
        return false;
    }

    return buffer->CreateBuffer();
}

void engine::MaterialData::UpdateBuffer() const
{
    if (buffer == nullptr)
    {
        return;
    }

    buffer->UpdateBuffer();
}

std::shared_ptr<DescriptorHandle> engine::MaterialData::UploadBuffer() const
{
    if (buffer == nullptr)
    {
        return nullptr;
    }

    return buffer->UploadBuffer();
}

CEREAL_REGISTER_TYPE(engine::MaterialData)