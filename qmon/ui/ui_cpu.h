#pragma once

#include "cfg.h"
#include "data.h"
#include "util.h"

#include "implot/implot.h"

namespace ui {
std::string CpuUsageText(const data::CPUMetric& m) {
    TCHAR buf[255];
    std::wstring all;
    swprintf_s(buf, 255, L"TEMP:%.1f℃ MAX:%.1f℃ MIN:%.1f℃\n", m.cpu_tempture, m.cpu_max_tempture, m.cpu_min_tempture);
    all += buf;

    int i = 0;
    for(auto &t : data::_cpu.max_proc) {
        swprintf_s(buf, 255, L"PID:%d USAGE:%.1f%% NAME:%s\n", t.pid, t.cpu_usage, t.name);
        i++;
        all += buf;
    }
    return util::ws2s(all);
}

// CPU使用率
void ShowCPUUsage() {
    using namespace uicfg;
    ImGui::Begin("CPU usage", &_cfg.show_cpu);
    auto &total_usage = data::_cpu.total_usage;
    const ImVec2 & last = total_usage.last();

    ImPlotFlags flags = 0;
    float x_history = last.x - 100; // 100
    static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
    ImPlot::SetNextPlotLimitsX(x_history, last.x, ImGuiCond_Always);
    ImPlot::SetNextPlotLimitsY(0, 100.f, ImGuiCond_Always);
    if(last.x > 0) {
        if(ImPlot::BeginPlot("##CpuUsage", NULL, NULL, ImVec2(-1, -1), flags, rt_axis, ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoLabel)) {
            ImPlot::SetLegendLocation(ImPlotLocation_NorthEast);
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, uicfg::_cfg.fill_alpha);
            ImPlot::PlotShaded("Usage", &total_usage.d[0].x, &total_usage.d[0].y, total_usage.d.size(), 0, total_usage.offset, 2 * sizeof(float));
            // 描边
            ImPlot::PlotLine("Usage", &total_usage.d[0].x, &total_usage.d[0].y, total_usage.d.size(), total_usage.offset, 2 * sizeof(float));

            // max usage process
            std::string text = CpuUsageText(data::_cpu);
            // 抵消掉文字自动根据pos上下左右居中的偏移
            // 参考PlotText的源码
            auto text_offset = ImGui::CalcTextSize(text.c_str()) * 0.5f;
            ImPlot::PushStyleColor(ImPlotCol_InlayText, ImVec4(0.75, 1, 0, 1));
            ImPlot::PlotText(text.c_str(), x_history + 1, 95.f, false, text_offset);
            ImPlot::PopStyleColor();

            ImPlot::EndPlot();
            ImPlot::PopStyleVar();
        }
    }
    ImGui::End();
}
}