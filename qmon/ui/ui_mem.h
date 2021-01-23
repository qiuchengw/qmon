#pragma once

#include "cfg.h"
#include "data.h"
#include "implot/implot.h"

namespace ui {

// 内存使用率
void ShowMemUsage() {
    using namespace uicfg;
    ImGui::Begin("MEM usage", &_cfg.show_mem);
    ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
    auto &used = data::_mem.used_bytes;

    static float history = 10.0f;
    ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");

    ImPlotFlags flags = 0;

    static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels;
    ImPlot::SetNextPlotLimitsX(0, data::_mem.max_bytes, ImGuiCond_Always);
    if(!used.Empty()) {
        if(ImPlot::BeginPlot("##Scrolling", data::_mem.xlabel, data::_mem.ylabel, ImVec2(-1, 150), flags, 0, ImPlotAxisFlags_LockMin)) {
            ImPlot::PlotShaded("USED", &used.d[0].x, &used.d[0].y, used.d.size(), 0, used.offset, 2 * sizeof(double));
            ImPlot::EndPlot();
        }
    }
    if(ImGui::Button("Close Me")) {
        _cfg.show_mem = false;
    }
    ImGui::End();
}
}