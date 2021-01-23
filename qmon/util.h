#pragma once

#include <sstream>
#include <iomanip>

namespace util {
    inline std::wstring Bytes2String(unsigned long long sz) {
        std::wostringstream ret;

        if (sz < 1024) ret << sz << L" B";
        else if (sz < 1024 * 1024) ret << std::setprecision(2) << std::fixed << sz / 1024.0 << L" KB";
        else if (sz < 1024 * 1024 * 1024)ret << std::setprecision(2) << std::fixed << sz / 1024.0 / 1024.0 << L" MB";
        else if (sz < 1024ULL * 1024 * 1024 * 1024)ret << std::setprecision(2) << std::fixed << sz / 1024.0 / 1024.0 / 1024.0 << L" GB";
        else ret << std::setprecision(2) << std::fixed << sz / 1024.0 / 1024.0 / 1024.0 / 1024.0 << L" TB";

        return ret.str();
    }

#define Bytes2MB(sz) ((double)(sz) / 1024 / 1024)
#define Bytes2GB(sz) ((double)(sz) / 1024 / 1024 / 1024)
}