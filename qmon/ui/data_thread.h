#pragma once

#include <thread>
#include <atomic>
#include "ui/data.h"
#include "util.h"
#include "sysrc/MemoryUsage.h"
#include "sysrc/CpuUsage.h"
#include "sysrc/CpuTemperature.h"

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

        CCpuUsage cpu_usage;
        if(!cpu_usage.Init()) {
            exit(-2);
        }

        CCpuTemperature cpu_temp;
        cpu_temp.Init();

        // !!!!data的访问是非线程安全，但是无所谓，偷点懒
        while(!_data_thread_stop) {
            mem.Update();
            cpu_usage.Update();
            cpu_temp.Update();

            // 刷新内存占用
            data::_mem.max_gb = Bytes2GB(mem.phys_total_bytes());
            auto tm_now = ImGui::GetTime();
            data::_mem.phys_used_gb.AddPoint(tm_now, Bytes2GB(mem.phys_used_bytes()));
            data::_mem.page_used_gb.AddPoint(tm_now, Bytes2GB(mem.page_used_bytes()));
            mem.CopyProcessT(data::_mem.max_proc, _countof(data::_mem.max_proc));

            // 刷新cpu占用
            data::_cpu.cpu_count = cpu_usage.CpuCount();
            data::_cpu.total_usage.AddPoint(tm_now, cpu_usage.GetValue());
            cpu_usage.CopyProcessT(data::_cpu.max_proc, _countof(data::_cpu.max_proc));

            // 刷新cpu温度
            data::_cpu.cpu_max_tempture = cpu_temp.MaxTempture();
            data::_cpu.cpu_min_tempture = cpu_temp.MinTempture();
            data::_cpu.cpu_tempture = cpu_temp.GetValue();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
    th.detach();
}

void stop_data_thread() {
    _data_thread_stop = true;
}
};