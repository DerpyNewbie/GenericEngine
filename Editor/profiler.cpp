#include "pch.h"

#include "profiler.h"

#include "engine_profiler.h"
#include "Rendering/crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

template <typename T>
struct ScrollingBuffer
{
    size_t max_size;
    int offset;
    std::vector<T> data;
    ScrollingBuffer(size_t max_size = 2000)
    {
        this->max_size = max_size;
        offset = 0;
        data.resize(max_size);
    }
    void Add(T value)
    {
        if (data.size() < max_size)
        {
            data.push_back(value);
            offset = static_cast<int>(data.size()) - 1;
        }
        else
        {
            data[offset] = value;
            offset = (offset + 1) % max_size;
        }
    }
    void Erase()
    {
        if (data.size() > 0)
        {
            data.clear();
            offset = 0;
        }
    }
};

static int g_history_data_size = 1000;
static float g_history_seconds = 5.0f;
static std::unordered_map<std::string, ScrollingBuffer<ImVec2>> g_history;
static auto g_history_sum = ScrollingBuffer<ImVec2>(g_history_data_size);
static auto g_gpu_history = ScrollingBuffer<ImVec2>(g_history_data_size);
static uint64_t g_last_gpu_frame = 0;

namespace editor
{
std::string Profiler::Name()
{
    return "Profiler";
}
void Profiler::OnEditorGui()
{
    if (ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const auto time = engine::Time::Get();
        ImGui::Indent();
        ImGui::Text("FPS        : %d", time->Fps());
        ImGui::Text("FPS Target : %d", time->FpsTarget());
        ImGui::Text("Frames     : %d", time->Frames());
        ImGui::Text("Time Since Start Up: %f", time->TimeSinceStartUp());
        ImGui::Text("Delta Time         : %f", time->DeltaTime());
        ImGui::Text("Unscaled Delta Time: %f", time->UnscaledDeltaTime());
        ImGui::Text("Time Scale         : %f", time->TimeScale());
        ImGui::Text("Last Fixed Frame Count: %d", time->LastFixedFrameCount());
        ImGui::Text("Renderer Count        : %d", static_cast<int>(engine::CrsBridge::Crs().Meshes().Count()));
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("GPU", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();
        bool gpu_enabled = engine::Profiler::IsGpuProfilingEnabled();
        if (ImGui::Checkbox("Enable GPU Profiler", &gpu_enabled))
        {
            engine::Profiler::SetGpuProfilingEnabled(gpu_enabled);
            g_gpu_history.Erase();
            g_gpu_history.data.resize(g_gpu_history.max_size);
            g_last_gpu_frame = 0;
        }

        if (gpu_enabled && engine::Profiler::has_gpu_profile)
        {
            const auto &profile = engine::Profiler::gpu_profile;
            const float t = engine::Time::Get()->TimeSinceStartUp();
            if (profile.frame_number != g_last_gpu_frame)
            {
                g_gpu_history.Add({t, static_cast<float>(profile.frame_gpu_ms / 1000.0)});
                g_last_gpu_frame = profile.frame_number;
            }

            ImGui::Text("GPU Frame  : %.3f ms", profile.frame_gpu_ms);
            ImGui::Text("Draw Calls : %u", profile.draw_calls);
            ImGui::Text("Frame No.  : %llu", static_cast<unsigned long long>(profile.frame_number));

            constexpr auto table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                         ImGuiTableFlags_SizingStretchProp;
            if (ImGui::BeginTable("##GpuScopes", 5, table_flags))
            {
                ImGui::TableSetupColumn("Scope", ImGuiTableColumnFlags_WidthStretch, 3.0f);
                ImGui::TableSetupColumn("GPU ms");
                ImGui::TableSetupColumn("Draws");
                ImGui::TableSetupColumn("Vertices");
                ImGui::TableSetupColumn("Indices");
                ImGui::TableHeadersRow();
                for (const auto &scope : profile.scopes)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Indent(static_cast<float>(scope.depth) * 12.0f + 0.001f);
                    ImGui::TextUnformatted(scope.name.c_str());
                    ImGui::Unindent(static_cast<float>(scope.depth) * 12.0f + 0.001f);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.3f", scope.gpu_ms);
                    ImGui::TableNextColumn();
                    ImGui::Text("%u", scope.draw_calls);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", static_cast<unsigned long long>(scope.vertices));
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", static_cast<unsigned long long>(scope.indices));
                }
                ImGui::EndTable();
            }
        }
        else if (gpu_enabled)
        {
            ImGui::TextUnformatted("Waiting for GPU results...");
        }
        ImGui::Unindent();
    }

    if (ImPlot::BeginPlot("##Profiler"))
    {
        const float t = engine::Time::Get()->TimeSinceStartUp();

        float total = 0.0f;
        for (auto &[name, duration] : engine::Profiler::prev_durations)
        {
            if (!g_history.contains(name))
            {
                g_history.emplace(name, ScrollingBuffer<ImVec2>(g_history_data_size));
                g_history[name].offset = g_history_sum.offset;
            }

            g_history[name].Add({t, duration});
            total += duration;
        }

        g_history_sum.Add({t, total});

        float max_y = 0.025f;
        if (engine::Profiler::IsGpuProfilingEnabled())
        {
            for (auto &[x, y] : g_gpu_history.data)
                max_y = (std::max)(max_y, y);
        }
        for (auto &[x, y] : g_history_sum.data)
        {
            if (y < max_y)
                continue;
            max_y = y;
        }

        ImPlot::SetupAxisLimits(ImAxis_X1, t - g_history_seconds, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, max_y, ImGuiCond_Always);

        for (auto &[name, history] : g_history)
        {
            ImPlot::PlotLine(
                name.c_str(),
                &history.data.data()[0].x,
                &history.data.data()[0].y,
                static_cast<int>(history.data.size()),
                0,
                history.offset,
                sizeof(float) * 2
            );
        }

        if (engine::Profiler::IsGpuProfilingEnabled())
        {
            ImPlot::PlotLine(
                "GPU Frame",
                &g_gpu_history.data.data()[0].x,
                &g_gpu_history.data.data()[0].y,
                static_cast<int>(g_gpu_history.data.size()),
                0,
                g_gpu_history.offset,
                sizeof(float) * 2
            );
        }

        ImPlot::PlotLine(
            "Total",
            &g_history_sum.data.data()[0].x,
            &g_history_sum.data.data()[0].y,
            static_cast<int>(g_history_sum.data.size()),
            0,
            g_history_sum.offset,
            sizeof(float) * 2
        );

        ImPlot::EndPlot();
    }
}
}