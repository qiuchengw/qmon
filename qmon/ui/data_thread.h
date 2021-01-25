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

// �����̷�
class Partion {
public:
    static std::vector<std::string> disk_partion() {
        DWORD dwLen = GetLogicalDriveStrings(0, NULL);	//��ȡϵͳ�ַ�������.
        char buf[1024] = {0}; // ��̫���ܳ���1024������
        GetLogicalDriveStringsA(dwLen, buf);		//��ȡ�̷��ַ���.

        std::vector<std::string> ret;
        char *p = buf;
        while(*p != '\0') {
            int a = strlen(p);
            ret.push_back(p);
            p += strlen(p) + 1;			//��λ����һ���ַ���.��һ��Ϊ������'\0'�ַ���.
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
            // todo: ֪ͨ
            exit(-1);
            // ...
        }

        CCpuUsage cpu_usage;
        if(!cpu_usage.Init()) {
            exit(-2);
        }

        CCpuTemperature cpu_temp;
        data::_cpu.has_tempture_feature = cpu_temp.Init();

        // !!!!data�ķ����Ƿ��̰߳�ȫ����������ν��͵����
        for(ULONGLONG idx = 0; !_data_thread_stop; idx++) {
            mem.Update();
            cpu_usage.Update();
            cpu_temp.Update();

            // ˢ���ڴ�ռ��
            data::_mem.max_gb = Bytes2GB(mem.phys_total_bytes());
            auto tm_now = ImGui::GetTime();
            data::_mem.phys_used_gb.AddPoint(tm_now, Bytes2GB(mem.phys_used_bytes()));
            data::_mem.page_used_gb.AddPoint(tm_now, Bytes2GB(mem.page_used_bytes()));
            mem.CopyProcessT(data::_mem.max_proc, _countof(data::_mem.max_proc));

            // ˢ��cpuռ��
            data::_cpu.cpu_count = cpu_usage.CpuCount();
            data::_cpu.total_usage.AddPoint(tm_now, cpu_usage.GetValue());
            cpu_usage.CopyProcessT(data::_cpu.max_proc, _countof(data::_cpu.max_proc));

            // ˢ��cpu�¶�
            if(data::_cpu.has_tempture_feature) {
                data::_cpu.cpu_max_tempture = cpu_temp.MaxTempture();
                data::_cpu.cpu_min_tempture = cpu_temp.MinTempture();
                data::_cpu.cpu_tempture = cpu_temp.GetValue();
            }

            // ˢ�´���ռ��
            if(idx % 10 == 0) {  // 10 * 0.5 s ˢ��һ��
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