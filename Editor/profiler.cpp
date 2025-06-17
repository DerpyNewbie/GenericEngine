#include "profiler.h"

#include "engine_profiler.h"
#include "implot.h"

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
            offset = data.size() - 1;
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

void Profiler::OnConstructed()
{
    IEditorWindow::OnConstructed();
    SetName("Profiler");
}
void Profiler::OnEditorGui()
{
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
            ImPlot::PlotLine(name.c_str(),
                             &history.data.data()[0].x, &history.data.data()[0].y,
                             static_cast<int>(history.data.size()), 0,
                             history.offset, sizeof(float) * 2);
        }

        ImPlot::PlotLine("Total",
                         &g_history_sum.data.data()[0].x, &g_history_sum.data.data()[0].y,
                         static_cast<int>(g_history_sum.data.size()), 0,
                         g_history_sum.offset, sizeof(float) * 2);

        ImPlot::EndPlot();
    }
}