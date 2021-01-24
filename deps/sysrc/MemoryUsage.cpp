#include "MemoryUsage.h"
#include<TlHelp32.h>
#include<Psapi.h>
#include"Log.h"

CMemoryUsage::CMemoryUsage()
    : m_hToken(nullptr)
    , m_bNotAllAssigned(false) {
    LOGOUT("CMemoryUsage构造");
}

CMemoryUsage::~CMemoryUsage() {
    if(m_hToken)
        CloseHandle(m_hToken);

    LOGOUT("CMemoryUsage析构");
}

bool CMemoryUsage::Init() {
    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &m_hToken)) {
        LOGERR("OpenProcessToken", GetLastError());
        return false;
    }

    LUID luid;

    if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
        LOGERR("LookupPrivilegeValue", GetLastError());
        return false;
    }

    TOKEN_PRIVILEGES tp = { 1 };
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if(!AdjustTokenPrivileges(m_hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
        LOGERR("AdjustTokenPrivileges", GetLastError());
        return false;
    }

    m_bNotAllAssigned = GetLastError() == ERROR_NOT_ALL_ASSIGNED;
    return true;
}

void CMemoryUsage::Update() {
    m_cur.dwLength = sizeof(m_cur);
    if(!GlobalMemoryStatusEx(&m_cur)) {
        LOGERR("GlobalMemoryStatusEx", GetLastError());
        return;
    }
    __LoopForProcesses();
}

void CMemoryUsage::Reset() {
    ZeroMemory(&m_cur, sizeof(m_cur));
}

void CMemoryUsage::__LoopForProcesses() {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if(INVALID_HANDLE_VALUE == hProcessSnap) {
        LOGERR("CreateToolhelp32Snapshot", GetLastError());
        return;
    }

    BOOL b;
    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };

    for (auto &x : m_maxProcesses) {
        x.bytes = 0;
    }

    for(b = Process32First(hProcessSnap, &pe32); b; b = Process32Next(hProcessSnap, &pe32)) {
        if(!pe32.th32ProcessID)continue;

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);

        if(!hProcess) {
            if(5 != GetLastError()) {
                LOGERR("OpenProcess", GetLastError());
                return;
            }
        } else {
            PROCESS_MEMORY_COUNTERS pmc = { sizeof(PROCESS_MEMORY_COUNTERS) };

            if(GetProcessMemoryInfo(hProcess, &pmc, sizeof pmc))
                for(size_t i = 0; i < _countof(m_maxProcesses); ++i)
                    if(m_maxProcesses[i].bytes < pmc.WorkingSetSize) {
                        for(size_t j = _countof(m_maxProcesses) - 1; j > i; --j)
                            m_maxProcesses[j] = m_maxProcesses[j - 1];

                        _tcscpy_s(m_maxProcesses[i].name, pe32.szExeFile);
                        m_maxProcesses[i].pid = pe32.th32ProcessID;
                        m_maxProcesses[i].bytes = pmc.WorkingSetSize;
                        break;
                    }

            CloseHandle(hProcess);
        }
    }

    CloseHandle(hProcessSnap);
}
