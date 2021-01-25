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

// 分区盘符
class Partion {
public:
    static std::vector<std::string> disk_partion() {
        DWORD dwLen = GetLogicalDriveStrings(0, NULL);	//获取系统字符串长度.
        char buf[1024] = {0}; // 不太可能超过1024个长度
        GetLogicalDriveStringsA(dwLen, buf);		//获取盘符字符串.

        std::vector<std::string> ret;
        char *p = buf;
        while(*p != '\0') {
            int a = strlen(p);
            ret.push_back(p);
            p += strlen(p) + 1;			//定位到下一个字符串.加一是为了跳过'\0'字符串.
        }
        return ret;
    }

    static ULARGE_INTEGER disk_usages(std::vector<PartionInfo>& parts) {
        parts.clear();
        ULARGE_INTEGER total = { 0 };
        for(auto& parti : disk_partion()) {
            PartionInfo pi;
            pi.part_name = parti;
            GetDiskFreeSpaceExA(parti.c_str(),
                                (PULARGE_INTEGER)&pi.free_user,
                                (PULARGE_INTEGER)&pi.total,
                                (PULARGE_INTEGER)&pi.free_total
                               );
            total.QuadPart += pi.total.QuadPart;
            parts.push_back(pi);
        }
        return total;
    }
};

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
        data::_cpu.has_tempture_feature = cpu_temp.Init();

        // !!!!data的访问是非线程安全，但是无所谓，偷点懒
        for(ULONGLONG idx = 0; !_data_thread_stop; idx++) {
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
            if(data::_cpu.has_tempture_feature) {
                data::_cpu.cpu_max_tempture = cpu_temp.MaxTempture();
                data::_cpu.cpu_min_tempture = cpu_temp.MinTempture();
                data::_cpu.cpu_tempture = cpu_temp.GetValue();
            }

            // 刷新磁盘占用
            if(idx % 10 == 0) {  // 10 * 0.5 s 刷新一次
                data::_disk.total = Partion::disk_usages(data::_disk.parts);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
    th.detach();
}

void stop_data_thread() {
    _data_thread_stop = true;
}
};