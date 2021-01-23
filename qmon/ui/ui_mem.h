#pragma once

#include "cfg.h"
#include "data.h"
#include "util.h"
#include "implot/implot.h"

namespace ui {

// 内存使用率
void ShowMemUsage() {
    using namespace uicfg;
    ImGui::Begin("MEM usage", &_cfg.show_mem);
    auto str = std::to_string(data::_mem.used_gb.last().y);
    ImGui::BulletText(str.c_str());
    ImGui::BulletText(data::_mem.str.c_str());
    auto &used = data::_mem.used_gb;

    static float history = 10.0f;
    ImGui::SliderFloat("History", &history, 1, 200, "%.1f s");

    ImPlotFlags flags = 0;
    static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels;
    //ImPlot::SetNextPlotLimitsX(0, data::_mem.max_bytes, ImGuiCond_Always);
    ImPlot::SetNextPlotLimitsX(ImGui::GetTime() - history, ImGui::GetTime(), ImGuiCond_Always);
    ImPlot::SetNextPlotLimitsY(0, data::_mem.max_gb, ImGuiCond_Always);
    if(!used.Empty()) {
        if(ImPlot::BeginPlot("##MemUsage", data::_mem.xlabel, data::_mem.ylabel, ImVec2(-1, -1), flags, 
            ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel, ImPlotAxisFlags_LockMin)) {
            ImPlot::PlotShaded("PhysUsed", &used.d[0].x, &used.d[0].y, used.d.size(), 0, used.offset, 2 * sizeof(float));
            ImPlot::EndPlot();
        }
    }
    if(ImGui::Button("Close Me")) {
        _cfg.show_mem = false;
    }
    ImGui::End();
}
}