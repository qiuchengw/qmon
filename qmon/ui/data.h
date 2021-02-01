#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "sysrc/MemoryUsage.h"
#include "sysrc/CpuUsage.h"
#include <mutex>
#include <limits>
#include <algorithm>

// ��������
namespace data {

// utility structure for realtime plot
// ����������
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

    bool MinMaxVal(float& min_val, float& max_val) {
        if(Empty()) {
            return false;
        }
        auto v = std::minmax_element(d.begin(), d.end(), [](auto & a, auto & b) {
            return a.y > b.y;
        });
        max_val = v.first->y;
        min_val = v.second->y;
        return true;
    }

    // fill_max_n : true, ��ȫ��ȫ��_MaxN ��Ԫ�أ���������0��
    ImVector<float> CopyY(int& size, bool fill_max_n = false) {
        ImVector<float> ret;
        size = d.size();
        if(d.size() < _MaxN) {
            if(fill_max_n) {
                // �Զ���0��
                ret.resize(_MaxN);
            }
            for(int i = 0; i < d.size(); ++i) {
                ret[i] = d[i].y;
            }
        } else {
            // ��ǰ���
            for(int i = offset; i < _MaxN; ++i) {
                ret.push_back(d[i].y);
            }
            // ���Σ�ǰ����ں���
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

// ��ǰʱ���
double _tm_now = 0; // ImGui::GetTime();

// �ڴ�ʹ����
struct MemMetric {
    const char* xlabel = "timeline";
    const char* ylabel = "GB";
    // �����ڴ��С
    float max_gb = 0;

    // �����ڴ���ʷʹ�õĴ�С
    ScrollingBuffer200 phys_used_gb;

    // �����ڴ���ʷʹ�õĴ�С
    ScrollingBuffer200 page_used_gb;

    // ��ǰ�ڴ�ʹ�������ļ������̣�
    ProcessMemInfoT max_proc[3] = { 0 };
} _mem;

// �ڴ�ʹ����
struct CPUMetric {
    // ������
    int cpu_count = 1;

    // �¶�
    bool has_tempture_feature = true;
    float cpu_tempture = 0.f;
    float cpu_max_tempture = 0.f;
    float cpu_min_tempture = 0.f;

    // ����cpuռ��
    ScrollingBuffer200 total_usage;

    // ����cpu(thread)ʹ����
    // size Ϊ cpu_count
    std::vector<ScrollingBuffer100> cpu_thread_usage;

    // ��ǰCPUʹ�������ļ������̣�
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
    // ����
    std::vector<PartionInfo> parts;

    // Read Bytes
    ScrollingBuffer100 read_bps;
    // Write bytes
    ScrollingBuffer100 write_bps;

    void MinMax(float &min_v, float &max_v) {
        read_bps.MinMaxVal(min_v, max_v);
        float min_v1, max_v1;
        write_bps.MinMaxVal(min_v1, max_v1);
        max_v = std::max(max_v, max_v1);
    }

} _disk;

// ��������ʹ����
struct EtherNet {
    // �����շ�����ͳ��
    ScrollingBuffer100 recv_bps;
    ScrollingBuffer100 send_bps;

    void MinMax(float &min_v, float &max_v) {
        recv_bps.MinMaxVal(min_v, max_v);
        float min_v1, max_v1;
        send_bps.MinMaxVal(min_v1, max_v1);
        max_v = std::max(max_v, max_v1);
    }
} _net;

}
