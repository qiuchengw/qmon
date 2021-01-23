#pragma once
#include "IMonitor.h"
#include<Windows.h>
#include<sstream>
#include<iomanip>

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

    double phys_total_bytes() const {
        return m_cur.ullTotalPhys;
    }

    double phys_available_bytes() const {
        return m_cur.ullAvailPhys;
    }

    double phys_used_bytes() const {
        return m_cur.ullTotalPhys * m_cur.dwMemoryLoad / 100;
    }

    double page_used_bytes() const {
        return m_cur.ullTotalPageFile - m_cur.ullAvailPageFile;
    }

    const std::string ToLongString()const {
        std::ostringstream ret;
        ret << "ava:" << phys_available_bytes()
            << " - used" << phys_used_bytes();
        return ret.str();
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

private:
    MEMORYSTATUSEX m_cur;
    ProcessT m_maxProcesses[3];
    HANDLE m_hToken;
    bool m_bNotAllAssigned;
};

