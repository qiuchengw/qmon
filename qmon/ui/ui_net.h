#pragma once

#include "cfg.h"
#include "data.h"
#include "util.h"

#include "implot/implot.h"

namespace ui {
// CPUÊ¹ÓÃÂÊ
void ShowNetUsage() {
    using namespace uicfg;
    ImGui::Begin("Net Traffic", &_cfg.show_network);
    auto &usage_recv = data::_net.recv_bps;
    const ImVec2 & last = usage_recv.last();

    if(last.x > 0) {
        float min_v, max_v;
        data::_net.MinMax(min_v, max_v);

        ImPlotFlags flags = 0;
        float x_history = last.x - 100; // 100
        static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
        ImPlot::SetNextPlotLimitsX(x_history, last.x, ImGuiCond_Always);
        ImPlot::SetNextPlotLimitsY(0, max_v, ImGuiCond_Always);

        if(ImPlot::BeginPlot("##Network", NULL, NULL, ImVec2(-1, -1), flags, rt_axis, ImPlotAxisFlags_LockMax | ImPlotAxisFlags_LockMax |  ImPlotAxisFlags_NoLabel)) {
            ImPlot::SetLegendLocation(ImPlotLocation_NorthEast);
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, uicfg::_cfg.plot_fill_alpha);
            // recv
            ImPlot::PlotShaded("recv(KB/s)", &usage_recv.d[0].x, &usage_recv.d[0].y, usage_recv.d.size(), 0, usage_recv.offset, 2 * sizeof(float));
            ImPlot::PlotLine("recv(KB/s)", &usage_recv.d[0].x, &usage_recv.d[0].y, usage_recv.d.size(), usage_recv.offset, 2 * sizeof(float));

            // recv
            auto &usage_send = data::_net.send_bps;
            ImPlot::PlotShaded("send(KB/s)", &usage_send.d[0].x, &usage_send.d[0].y, usage_send.d.size(), 0, usage_send.offset, 2 * sizeof(float));
            ImPlot::PlotLine("send(KB/s)", &usage_send.d[0].x, &usage_send.d[0].y, usage_send.d.size(), usage_send.offset, 2 * sizeof(float));

            ImPlot::EndPlot();
            ImPlot::PopStyleVar();
        }
    }
    ImGui::End();
}

}