#pragma once

#include <stdint.h>
#include <string>
#include "sysrc/MemoryUsage.h"
#include "sysrc/CpuUsage.h"

// 数据中心
namespace data {

// utility structure for realtime plot
// 滚动的数据
struct ScrollingBuffer {
    int max_n;
    int offset;
    ImVector<ImVec2> d;

    ScrollingBuffer() {
        max_n = 200;
        offset = 0;
        d.reserve(max_n);
    }

    bool Empty() const {
        return d.size() <= 0;
    }

    void AddPoint(float x, float y) {
        if(d.size() < max_n) {
            d.push_back(ImVec2(x, y));
        } else {
            d[offset] = ImVec2(x, y);
            offset = (offset + 1) % max_n;
        }
    }

    ImVec2 last() const {
        if(Empty()) {
            return ImVec2(0, 0);
        }

        if(d.size() < max_n) {
            return d[d.size() - 1];
        }
        if(offset == 0) {
            return d[offset];
        }
        return d[offset - 1];
    }

    void Erase() {
        if(d.size() > 0) {
            d.shrink(0);
            offset = 0;
        }
    }
};

// 内存使用率
struct MemMetric {
    const char* xlabel = "timeline";
    const char* ylabel = "GB";
    float max_gb = 0; // 大小

    // 物理内存历史使用的大小
    ScrollingBuffer phys_used_gb;

    // 物理内存历史使用的大小
    ScrollingBuffer page_used_gb;

    // 当前内存使用率最大的几个进程！
    ProcessMemInfoT max_proc[3] = { 0 };
} _mem;

// 内存使用率
struct CPUMetric {
    // 核心数
    int cpu_count = 1;

    // 温度
    bool has_tempture_feature = true;
    float cpu_tempture = 0.f;
    float cpu_max_tempture = 0.f;
    float cpu_min_tempture = 0.f;

    // 整体cpu占用
    ScrollingBuffer total_usage;

    // 当前CPU使用率最大的几个进程！
    ProcessCpuInfoT max_proc[3] = { 0 };
} _cpu;

}
