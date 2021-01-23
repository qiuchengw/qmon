#pragma once
#include "IMonitor.h"
#include<Windows.h>

// 进程信息
struct ProcessT{
    DWORD pid;
    TCHAR name[MAX_PATH];
    size_t size;
};

class CMemoryUsage :
    public IMonitor {
public:
    CMemoryUsage();
    ~CMemoryUsage();

    const double GetValue()const;
    const double Max()const {
        return m_dwMax;
    }
    const double Min()const {
        return m_dwMin;
    }

    const MEMORYSTATUSEX &metric() {
        return m_cur;
    }

    void CopyProcessT(ProcessT p[3], unsigned int n) {
        if (n > 3) {
            n = 3;
        }

        for (unsigned int i = 0; i < n; i++) {
            CopyMemory(p + i, m_maxProcesses + i, sizeof(ProcessT));
        }
    }

    bool Init();
    void Update();
    void Reset();
private:
    void __LoopForProcesses();
    const std::wstring __Bytes2String(unsigned long long)const;

private:
    DWORD m_dwMin, m_dwMax;
    MEMORYSTATUSEX m_cur;
    ProcessT m_maxProcesses[3];
    HANDLE m_hToken;
    bool m_bNotAllAssigned;
};

