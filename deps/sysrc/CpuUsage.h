#pragma once
#include "IMonitor.h"
#include<Windows.h>
#include<winternl.h>
#include<map>

typedef struct tagPROCESS_INFO {
    DWORD pid;
    TCHAR name[MAX_PATH];
    unsigned long long last_time, last_system_time;
    double cpu_usage;
} ProcessCpuInfoT;

#define MAX_CPU_USAGE_PROCESS 3

class CCpuUsage :
    public IMonitor {

public:
    CCpuUsage();
    ~CCpuUsage();

    const std::basic_string<TCHAR> ToString()const;
    const std::wstring ToLongString()const;
    const double GetValue()const;
    bool Init() {
        return true;
    }
    void Update();
    void Reset();

    inline DWORD CpuCount() const {
        return dwNUMBER_OF_PROCESSORS;
    }

    void CopyProcessT(ProcessCpuInfoT p[3], unsigned int n) {
        if (n > MAX_CPU_USAGE_PROCESS) {
            n = MAX_CPU_USAGE_PROCESS;
        }

        for (unsigned int i = 0; i < n; i++) {
            CopyMemory(p + i, &(m_pairMaxProcesses[i].second), sizeof(ProcessCpuInfoT));
        }
    }

private:
    unsigned long long __FileTime2Utc(const FILETIME &);
    double __GetCpuUsage(unsigned long long &, unsigned long long &);
    double __GetCpuUsage(HANDLE, unsigned long long &, unsigned long long &);
    void __LoopForProcesses();
    const DWORD __GetCpuConut()const;

private:
    double m_dMax, m_dMin, m_dCur;
    const DWORD dwNUMBER_OF_PROCESSORS;
    unsigned long long m_ullLastTime, m_ullLastIdleTime;
    std::map<DWORD, ProcessCpuInfoT> m_mapProcessMap;
    std::pair<DWORD, ProcessCpuInfoT> m_pairMaxProcesses[MAX_CPU_USAGE_PROCESS];
};

