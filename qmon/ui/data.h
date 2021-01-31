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
template<int _MaxN = 200>
class ScrollingBufferT {
public:
    int offset;
    ImVector<ImVec2> d;

    ScrollingBufferT() {
        offset = 0;
        d.reserve(_MaxN);
    }

    bool Empty() const {
        return d.size() <= 0;
    }

    // fill_max_n : true, 补全到全部_MaxN 个元素（不够的填0）
    ImVector<float> CopyY(int& size, bool fill_max_n = false) {
        ImVector<float> ret;
        size = d.size();
        if(d.size() < _MaxN) {
            if(fill_max_n) {
                // 自动补0了
                ret.resize(_MaxN);
            }
            for(int i = 0; i < d.size(); ++i) {
                ret[i] = d[i].y;
            }
        } else {
            // 最前面的
            for(int i = offset; i < _MaxN; ++i) {
                ret.push_back(d[i].y);
            }
            // 环形，前面的在后面
            for(int i = 0; i < offset; ++i) {
                ret.push_back(d[i].y);
            }
        }
        return ret;
    }

    void AddPoint(float x, float y) {
        if(d.size() < _MaxN) {
            d.push_back(ImVec2(x, y));
        } else {
            d[offset] = ImVec2(x, y);
            offset = (offset + 1) % _MaxN;
        }
    }

    ImVec2 last() const {
        if(Empty()) {
            return ImVec2(0, 0);
        }

        if(d.size() < _MaxN) {
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

typedef ScrollingBufferT<200> ScrollingBuffer200;
typedef ScrollingBufferT<100> ScrollingBuffer100;
typedef ScrollingBufferT<50> ScrollingBuffer50;

// 数据保护
std::mutex _data_mtx;

// 内存使用率
struct MemMetric {
    const char* xlabel = "timeline";
    const char* ylabel = "GB";
    // 物理内存大小
    float max_gb = 0;

    // 物理内存历史使用的大小
    ScrollingBuffer200 phys_used_gb;

    // 物理内存历史使用的大小
    ScrollingBuffer200 page_used_gb;

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
    ScrollingBuffer200 total_usage;

    // 单个cpu(thread)使用率
    // size 为 cpu_count
    std::vector<ScrollingBuffer100> cpu_thread_usage;

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
