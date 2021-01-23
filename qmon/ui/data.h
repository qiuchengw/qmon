#pragma once

#include <stdint.h>
#include <string>
#include "sysrc/MemoryUsage.h"

// ��������
namespace data {

// utility structure for realtime plot
// ����������
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
        if (d.size() < max_n) {
            d.push_back(ImVec2(x, y));
        }
        else {
            d[offset] = ImVec2(x, y);
            offset = (offset + 1) % max_n;
        }
    }

    ImVec2 last() const {
        if (Empty()) {
            return ImVec2(0, 0);
        }

        if (d.size() < max_n) {
            return d[d.size() - 1];
        }
        if (offset == 0) {
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

// �ڴ�ʹ����
struct MemMetric {
    const char* xlabel = "timeline";
    const char* ylabel = "MB";
    uint64_t max_gb = 0; // ��С
    std::string str;

    // ��ʷʹ�õĴ�С
    ScrollingBuffer used_gb;

    // ��ǰ��״̬
    MEMORYSTATUSEX status;

    // ��ǰʹ�������ļ������̣�
    ProcessT max_proc[3] = { 0 };
} _mem;

}
