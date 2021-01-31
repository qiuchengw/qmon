#pragma once

#include "cfg.h"
#include "data.h"
#include "util.h"

#include "implot/implot.h"

namespace ui {
std::string CpuUsageText(const data::CPUMetric& m) {
    TCHAR buf[255];
    std::wstring all;
    if(data::_cpu.has_tempture_feature) {
        swprintf_s(buf, 255, L"TEMP:%.1f℃ MAX:%.1f℃ MIN:%.1f℃\n", m.cpu_tempture, m.cpu_max_tempture, m.cpu_min_tempture);
        all += buf;
    }

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
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, uicfg::_cfg.plot_fill_alpha);
            ImPlot::PlotShaded("Usage(%)", &total_usage.d[0].x, &total_usage.d[0].y, total_usage.d.size(), 0, total_usage.offset, 2 * sizeof(float));
            // 描边
            ImPlot::PlotLine("Usage(%)", &total_usage.d[0].x, &total_usage.d[0].y, total_usage.d.size(), total_usage.offset, 2 * sizeof(float));

            // max usage process
            std::string text = CpuUsageText(data::_cpu);
            // 抵消掉文字自动根据pos上下左右居中的偏移
            // 参考PlotText的源码
            auto text_offset = ImGui::CalcTextSize(text.c_str()) * 0.5f;
            ImPlot::PushStyleColor(ImPlotCol_InlayText, uicfg::_cfg.color_plot_inlay_text);
            ImPlot::PlotText(text.c_str(), x_history + 1, 95.f, false, text_offset);
            ImPlot::PopStyleColor();

            ImPlot::EndPlot();
            ImPlot::PopStyleVar();
        }
    }
    ImGui::End();
}

// Example for Tables section. Generates a quick and simple shaded line plot. See implementation at bottom.
void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size) {
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
    ImPlot::SetNextPlotLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
    if(ImPlot::BeginPlot(id, 0, 0, size, ImPlotFlags_CanvasOnly | ImPlotFlags_NoChild, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations)) {
        ImPlot::PushStyleColor(ImPlotCol_Line, col);
        ImPlot::PlotLine(id, values, count, 1, 0, offset);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, uicfg::_cfg.plot_fill_alpha);
        ImPlot::PlotShaded(id, values, count, 0, 1, 0, offset);
        ImPlot::PopStyleVar();
        ImPlot::PopStyleColor();
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
}

// 单个Thread的cpu使用率
void ShowCpuThreadUsage() {
    if((data::_cpu.cpu_thread_usage.size() <= 0) || (data::_cpu.cpu_thread_usage[0].last().x <= 0)) {
        return;
    }

    ImGui::Begin("CPU Thread", &uicfg::_cfg.show_cpu_thread);
    ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg ;
    if(ImGui::BeginTable("##cpu_table", 3, flags, ImVec2(-1, 0))) {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 22.0f);
        ImGui::TableSetupColumn("Load", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Usage");
        ImGui::TableHeadersRow();
        ImPlot::PushColormap(ImPlotColormap_Cool);
        for(int row = 0; row < data::_cpu.cpu_count; row++) {
            int size = 0;
            auto usage = data::_cpu.cpu_thread_usage[row].CopyY(size, true);
            if(usage[size - 1] >= uicfg::_cfg.cpu_percent_lg) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("#%d", row);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.1f%%", usage[size - 1]);
                ImGui::TableSetColumnIndex(2);
                ImGui::PushID(row);
                Sparkline("##spark_cpu_usage", &(usage[0]), usage.size(), 0, 100.0f, 0, ImPlot::GetColormapColor(row), ImVec2(-1, 35));
                ImGui::PopID();
            }
        }
        ImPlot::PopColormap();
        ImGui::EndTable();
    }
    ImGui::End();
}
}