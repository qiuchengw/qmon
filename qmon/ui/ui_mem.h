#pragma once

#include "cfg.h"
#include "data.h"
#include "util.h"
#include "implot/implot.h"

namespace ui {

std::string MemUsageText(const data::MemMetric& m) {
    TCHAR buf[255];
    std::wstring all;
    swprintf_s(buf, 255, L"TOTAL:%.2f GB\n", m.max_gb);
    all += buf;

    int i = 0;
    for(auto &t : data::_mem.max_proc) {
        swprintf_s(buf, 255, L"PID:%d MEM:%.2fMB NAME:%s\n", t.pid, Bytes2MB(t.bytes), t.name);
        i++;
        all += buf;
    }
    return util::ws2s(all);
}

// 内存使用率
void ShowMemUsage() {
    using namespace uicfg;
    ImGui::Begin("MEM usage", &_cfg.show_mem);
    const ImVec2 & last = data::_mem.phys_used_gb.last();
    auto str = std::to_string(last.y);
    // ImGui::BulletText(str.c_str());
    // ImGui::BulletText(data::_mem.str.c_str());
    auto &phys_used = data::_mem.phys_used_gb;
    auto &virtual_used = data::_mem.page_used_gb;

    // static float history = 100.0f;
    // ImGui::SliderFloat("History", &history, 1, 200, "%.1f s");

    ImPlotFlags flags = 0;
    static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
    float x_history = last.x - 100; // 100
    ImPlot::SetNextPlotLimitsX(x_history, last.x, ImGuiCond_Always);
    ImPlot::SetNextPlotLimitsY(0, data::_mem.max_gb, ImGuiCond_Always);
    if(last.x > 0) {
        if(ImPlot::BeginPlot("##MemUsage", data::_mem.xlabel, data::_mem.ylabel, ImVec2(-1, -1), flags,
                             rt_axis, ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoLabel)) {
            ImPlot::SetLegendLocation(ImPlotLocation_NorthEast);
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, uicfg::_cfg.fill_alpha);
            // 描边
            ImPlot::PlotShaded("CommitUsed", &virtual_used.d[0].x, &virtual_used.d[0].y, virtual_used.d.size(), 0, virtual_used.offset, 2 * sizeof(float));
            ImPlot::PlotLine("CommitUsed", &virtual_used.d[0].x, &virtual_used.d[0].y, virtual_used.d.size(), virtual_used.offset, 2 * sizeof(float));
            ImPlot::PlotShaded("PhysUsed", &phys_used.d[0].x, &phys_used.d[0].y, phys_used.d.size(), 0, phys_used.offset, 2 * sizeof(float));
            ImPlot::PlotLine("PhysUsed", &phys_used.d[0].x, &phys_used.d[0].y, phys_used.d.size(), phys_used.offset, 2 * sizeof(float));

            // max usage process
            std::string text = MemUsageText(data::_mem);
            // 抵消掉文字自动根据pos上下左右居中的偏移
            auto text_offset = ImGui::CalcTextSize(text.c_str()) * 0.5f;
            ImPlot::PushStyleColor(ImPlotCol_InlayText, ImVec4(0.75, 1, 0, 1));
            ImPlot::PlotText(text.c_str(), x_history + 1, data::_mem.max_gb * 0.95f, false, text_offset);
            ImPlot::PopStyleColor();

            ImPlot::EndPlot();
            ImPlot::PopStyleVar();
        }
    }
    ImGui::End();
}
}