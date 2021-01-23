#pragma once

#include <thread>
#include <atomic>
#include "ui/data.h"
#include "util.h"
#include "sysrc/MemoryUsage.h"

namespace data {
static std::atomic_bool _data_thread_stop = false;

void run_data_thread() {
    std::thread th([&]() {
        CMemoryUsage mem;
        if(!mem.Init()) {
            // todo: 通知
            exit(-1);
            // ...
        }

        double idx = 0;
        // !!!!data的访问是非线程安全，但是无所谓，偷点懒
        while(!_data_thread_stop) {
            mem.Update();

            idx++;
            data::_mem.str = mem.ToLongString();
            // 刷新内存占用
            data::_mem.max_gb = Bytes2GB(mem.phys_total_bytes());
            auto tm_now = ImGui::GetTime();
            data::_mem.used_gb.AddPoint(idx, Bytes2GB(mem.phys_used_bytes()));
            // data::_mem.used_gb.AddPoint(idx, (uint64_t)idx % 50);
            mem.CopyProcessT(data::_mem.max_proc, sizeof(data::_mem.max_proc) / sizeof(data::_mem.max_proc[0]));

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
    th.detach();
}

void stop_data_thread() {
    _data_thread_stop = true;
}
};