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
    float fill_alpha = 0.5f;
    ImVec4 color_plot_inlay_text = ImVec4(0.75, 1, 0, 1);
} _cfg;
}