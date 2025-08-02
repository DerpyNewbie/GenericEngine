#include "pch.h"
#include "audio_window.h"

#include "gui.h"
#include "Audio/audio.h"

namespace editor
{
std::string AudioWindow::Name()
{
    return "Audio";
}
void AudioWindow::OnEditorGui()
{
    auto audio = engine::Audio::Instance();
    if (ImGui::CollapsingHeader("Audio Listener"))
    {
        const auto audio_listener = audio->AudioListener();
        const auto position = audio_listener.Position;
        const auto front = audio_listener.OrientFront;
        const auto top = audio_listener.OrientTop;

        ImGui::Indent();
        ImGui::Text("Position: {%.2f, %.2f, %.2f}", position.x, position.y, position.z);
        ImGui::Text("Front   : {%.2f, %.2f, %.2f}", front.x, front.y, front.z);
        ImGui::Text("Top     : {%.2f, %.2f, %.2f}", top.x, top.y, top.z);
        ImGui::Unindent();
    }

    auto volume = audio->MasterVolume();
    if (engine::Gui::PropertyField("Master Volume", volume))
    {
        audio->SetMasterVolume(volume);
    }

    auto doppler = audio->DopplerFactor();
    if (engine::Gui::PropertyField("Doppler Factor", doppler))
    {
        audio->SetDopplerFactor(doppler);
    }
}
}