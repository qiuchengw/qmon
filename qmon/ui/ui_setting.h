#pragma once

#include "cfg.h"

namespace ui {
void ShowSettingBox() {
    ImGui::Begin("Settings");
    ImGui::CheckboxFlags("CPU", (unsigned int*) & (uicfg::_cfg.show_cpu), 1);
    ImGui::SameLine();
    ImGui::CheckboxFlags("MEM", (unsigned int*) & (uicfg::_cfg.show_mem), 1);
    ImGui::SameLine();
    ImGui::CheckboxFlags("DISK", (unsigned int*) & (uicfg::_cfg.show_disk), 1);
    ImGui::SameLine();
    ImGui::CheckboxFlags("TRAFFIC", (unsigned int*) & (uicfg::_cfg.show_network), 1);

    ImGui::SliderFloat("Plot Fill", &(uicfg::_cfg.plot_fill_alpha), 0.f, 1.f);
    ImGui::ColorEdit4("Bkgnd", (float*) & (uicfg::_cfg.color_bkgnd), ImGuiColorEditFlags_NoInputs); // Edit 3 floats representing a color
    ImGui::SameLine();
    ImGui::ColorEdit4("Plot Text", (float*) & (uicfg::_cfg.color_plot_inlay_text), ImGuiColorEditFlags_NoInputs); // Edit 3 floats representing a color

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

}