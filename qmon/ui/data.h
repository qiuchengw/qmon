#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "sysrc/MemoryUsage.h"
#include "sysrc/CpuUsage.h"
#include <mutex>

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

// 数据保护
std::mutex _data_mtx;

// 内存使用率
struct MemMetric {
    const char* xlabel = "timeline";
    const char* ylabel = "GB";
    // 物理内存大小
    float max_gb = 0;

    // 物理内存历史使用的大小
    ScrollingBuffer phys_used_gb;

    // 物理内存历史使用的大小
    ScrollingBuffer page_used_gb;

    // 当前内存使用率最大的几个进程！
    ProcessMemInfoT max_proc[3] = { 0 };
} _mem;

// 内存使用率
struct CPUMetric {
    // 当前时间点
    double tm_now = 0; // ImGui::GetTime();

    // 核心数
    int cpu_count = 1;

    // 温度
    bool has_tempture_feature = true;
    float cpu_tempture = 0.f;
    float cpu_max_tempture = 0.f;
    float cpu_min_tempture = 0.f;

    // 整体cpu占用
    ScrollingBuffer total_usage;

    // 单个cpu(thread)使用率
    // size 为 cpu_count
    std::vector<ScrollingBuffer> cpu_thread_usage;

    // 当前CPU使用率最大的几个进程！
    ProcessCpuInfoT max_proc[3] = { 0 };
} _cpu;

struct PartionInfo {
    std::string part_name;
    ULARGE_INTEGER free_user;
    ULARGE_INTEGER total;
    ULARGE_INTEGER free_total;
};

struct DiskMetric {
    ULARGE_INTEGER total;
    // 分区
    std::vector<PartionInfo> parts;
} _disk;

}
