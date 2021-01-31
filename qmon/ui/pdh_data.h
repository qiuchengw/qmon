#pragma once

#include <vector>
#include <functional>
#include "pdh.h"
#include "data.h"
#include "util.h"
#include "sysrc/MemoryUsage.h"
#include "sysrc/CpuUsage.h"
#include "sysrc/CpuTemperature.h"

namespace data {
// Ӳ�̷����̷�
class Partion {
public:
    static std::vector<std::string> disk_partion() {
        DWORD dwLen = GetLogicalDriveStrings(0, NULL);	//��ȡϵͳ�ַ�������.
        char buf[1024] = { 0 }; // ��̫���ܳ���1024������
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

// �����ռ�
class CPdhData {
public:
    // ���ܼ����Ŀ
    BOOL Init() {
        if(!mem_.Init()) {
            return FALSE;
        }

        if(!cpu_.Init()) {
            return FALSE;
        }
        data::_cpu.cpu_count = cpu_.CpuCount();
        data::_cpu.cpu_thread_usage.resize(data::_cpu.cpu_count);
        data::_cpu.has_tempture_feature = cpu_temp_.Init();

        // PDH
        if(!phd_.Init()) {
            return FALSE;
        }

        // ��ʼ��cpuʹ��
        WCHAR buf[MAX_PATH] = { 0 };
        for(int i = 0; i < data::_cpu.cpu_count; i++) {
            swprintf_s(buf, MAX_PATH, L"\\Processor(%d)\\%% Processor Time\0", i);
            AddPdhCallback(buf, [ = ](double val) {
                data::_cpu.cpu_thread_usage[i].AddPoint(data::_tm_now, val);
            });
        }

        // ��ʼ�������շ�
        AddPdhCallback(df_PDH_ETHERNETRECV_BYTES, [ = ](double val) {
            data::_net.recv_bps.AddPoint(data::_tm_now, Bytes2KB(val));
        });
        AddPdhCallback(df_PDH_ETHERNETSEND_BYTES, [ = ](double val) {
            data::_net.send_bps.AddPoint(data::_tm_now, Bytes2KB(val));
        });


        // ����
        AddPdhCallback(df_PDH_DISKREAD_BYTES, [ = ](double val) {
            data::_disk.read_bps.AddPoint(data::_tm_now, Bytes2KB(val));
        });
        AddPdhCallback(df_PDH_DISKWRITE_BYTES, [ = ](double val) {
            data::_disk.write_bps.AddPoint(data::_tm_now, Bytes2KB(val));
        });
        return TRUE;
    }

    void Collect() {
        SysUsageRefresh();

        if(ERROR_SUCCESS != phd_.CollectQueryData()) {
            return;
        }

        // ˢ��һ��
        for(auto fn : getters_) {
            fn();
        }

    }

    // һ�������ݵĻ�ȡʹ��sysrcʵ��
    // �ⲿ�ִ���߱���ȡ��ǰʹ��cpu��mem����������̵Ĺ��ܣ�������
    // ���뵼�²�ͳһ�����Ǽ���͵��ʹ��
    void SysUsageRefresh() {
        mem_.Update();
        cpu_.Update();
        cpu_temp_.Update();

        // ˢ���ڴ�ռ��
        data::_tm_now = ImGui::GetTime();
        data::_mem.max_gb = Bytes2GB(mem_.phys_total_bytes());
        data::_mem.phys_used_gb.AddPoint(data::_tm_now, Bytes2GB(mem_.phys_used_bytes()));
        data::_mem.page_used_gb.AddPoint(data::_tm_now, Bytes2GB(mem_.page_used_bytes()));
        mem_.CopyProcessT(data::_mem.max_proc, _countof(data::_mem.max_proc));

        // ˢ��cpuռ��
        // data::_cpu.cpu_count = cpu_.CpuCount();
        data::_cpu.total_usage.AddPoint(_tm_now, cpu_.GetValue());
        cpu_.CopyProcessT(data::_cpu.max_proc, _countof(data::_cpu.max_proc));

        // ˢ��cpu�¶�
        if(data::_cpu.has_tempture_feature) {
            data::_cpu.cpu_max_tempture = cpu_temp_.MaxTempture();
            data::_cpu.cpu_min_tempture = cpu_temp_.MinTempture();
            data::_cpu.cpu_tempture = cpu_temp_.GetValue();
        }

        // ˢ�´���ռ��
        data::_disk.total = Partion::disk_usages(data::_disk.parts);
    }

protected:
    inline BOOL AddPdhCallback(const WCHAR* item, std::function<void(double val)> cb) {
        int idx;
        if(ERROR_SUCCESS == phd_.AddCounter(item, idx)) {
            auto func = [&](int _idx, std::function<void(double val)> _cb) {
                double val = 0;
                if(phd_.GetCounterValue(_idx, &val)) {
                    _cb(val);
                }
            };
            getters_.push_back(std::bind(func, idx, cb));
            return TRUE;
        }
        return FALSE;
    }

    inline BOOL AddPdhVal(const WCHAR* item, double& val) {
        int idx;
        if(ERROR_SUCCESS == phd_.AddCounter(item, idx)) {
            auto func = [&](int i) {
                if(phd_.GetCounterValue(i, &val)) {
                    // ��ȡֵ
                }
            };
            getters_.push_back(std::bind(func, idx));
            return TRUE;
        }
        return FALSE;
    }

private:
    CPDH phd_;
    CMemoryUsage mem_;
    CCpuUsage cpu_;
    CCpuTemperature cpu_temp_;

    typedef std::function<void()> _DataGetter;
    std::vector<_DataGetter> getters_;
};
}