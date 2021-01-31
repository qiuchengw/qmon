#pragma once

#include "cfg.h"
#include "data.h"
#include "util.h"

#include "implot/implot.h"

namespace ui {
// CPU π”√¬ 
void ShowDiskUsage() {
    std::lock_guard<std::mutex> lg(data::_data_mtx);

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
        data2[i] = Bytes2GB(d[i].total.QuadPart);
    }
//     labels2[1] = "D:\\";
//     data2[1] = 300;

    ImPlot::PushColormap(ImPlotColormap_Deep);
    ImPlot::SetNextPlotLimits(0, 1, 0, 1, ImGuiCond_Always);
    if(ImPlot::BeginPlot("##Pie222", NULL, NULL, ImVec2(250, 250),
                         ImPlotFlags_Equal | ImPlotFlags_NoMousePos | ImPlotFlags_AntiAliased,
                         ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations)) {
        ImPlot::PlotPieChart(labels2, data2, N_X, 0.5, 0.5, 0.5, true, "%.0f GB", 180);
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();
    ImGui::End();
}
}