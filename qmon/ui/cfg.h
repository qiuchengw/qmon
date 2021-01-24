#pragma once

namespace uicfg {
struct Styles {
};

struct Config {
    bool show_mem = true;
    bool show_cpu = true;
    bool show_disk = true;
    bool show_network = true;

    // styles
    float plot_fill_alpha = 0.5f;
    ImVec4 color_plot_inlay_text = ImVec4(0.75, 1, 0, 1);
    ImVec4 color_bkgnd = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

} _cfg;
}