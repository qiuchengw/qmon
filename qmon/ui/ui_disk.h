#pragma once

#include "cfg.h"
#include "data.h"
#include "util.h"

#include "implot/implot.h"

namespace ui {
// CPUÊ¹ÓÃÂÊ
void ShowDiskUsage() {
    using namespace uicfg;
    auto& d = data::_disk.parts;
    if(d.size() == 0) {
        return;
    }

    ImGui::Begin("Disk usage", &_cfg.show_disk);
    static const int MAX_PARTS = 32;
    static const int MAX_PARTS_LEN = 16;
    static const int N_X = (d.size() > MAX_PARTS) ? MAX_PARTS : d.size();
    // char labels2[MAX_PARTS][MAX_PARTS_LEN] = { 0 };
    const char *labels2[MAX_PARTS] = { 0 };
    double data2[MAX_PARTS] = { 0 };
    for(int i = 0; i < N_X; ++i) {
        labels2[i] = d[i].part_name.c_str();
        // strncpy_s(labels2[i], MAX_PARTS_LEN, d[i].part_name.c_str(), d[i].part_name.length());
        // data2[i] = (double)(d[i].total.QuadPart) / (double)data::_disk.total.QuadPart;
    }

    auto &usage_read = data::_disk.read_bps;
    const ImVec2 & last = usage_read.last();
    if(last.x > 0) {
        float min_v, max_v;
        data::_disk.MinMax(min_v, max_v);

        ImPlotFlags flags = 0;
        float x_history = last.x - 100; // 100
        static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
        ImPlot::SetNextPlotLimitsX(x_history, last.x, ImGuiCond_Always);
        ImPlot::SetNextPlotLimitsY(0, max_v, ImGuiCond_Always);

        if(ImPlot::BeginPlot("##DiskUsage", NULL, NULL, ImVec2(-1, -1), flags, rt_axis, ImPlotAxisFlags_LockMax | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoLabel)) {
            ImPlot::SetLegendLocation(ImPlotLocation_NorthEast);
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, uicfg::_cfg.plot_fill_alpha);
            // recv
            ImPlot::PlotShaded("read(KB/s)", &usage_read.d[0].x, &usage_read.d[0].y, usage_read.d.size(), 0, usage_read.offset, 2 * sizeof(float));
            ImPlot::PlotLine("read(KB/s)", &usage_read.d[0].x, &usage_read.d[0].y, usage_read.d.size(), usage_read.offset, 2 * sizeof(float));

            // recv
            auto &usage_write = data::_disk.write_bps;
            ImPlot::PlotShaded("write(KB/s)", &usage_write.d[0].x, &usage_write.d[0].y, usage_write.d.size(), 0, usage_write.offset, 2 * sizeof(float));
            ImPlot::PlotLine("write(KB/s)", &usage_write.d[0].x, &usage_write.d[0].y, usage_write.d.size(), usage_write.offset, 2 * sizeof(float));

            ImPlot::EndPlot();
            ImPlot::PopStyleVar();
        }
    }

//     ImPlot::PushColormap(ImPlotColormap_Deep);
//     ImPlot::SetNextPlotLimits(0, 1, 0, 1, ImGuiCond_Always);
//     if(ImPlot::BeginPlot("##Pie222", NULL, NULL, ImVec2(-1, -1),
//                          ImPlotFlags_Equal | ImPlotFlags_NoMousePos | ImPlotFlags_AntiAliased,
//                          ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations)) {
//         ImPlot::PlotPieChart(labels2, data2, N_X, 0.5, 0.5, 0.5, true, "%.0f GB", 180);
//         ImPlot::EndPlot();
//     }
//
//     ImPlot::PopColormap();
    ImGui::End();
}
}