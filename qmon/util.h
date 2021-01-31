#pragma once

#include <sstream>
#include <iomanip>
#include <locale>
#include <codecvt>

namespace util {
inline std::wstring Bytes2String(unsigned long long sz) {
    std::wostringstream ret;

    if(sz < 1024) ret << sz << L" B";
    else if(sz < 1024 * 1024) ret << std::setprecision(2) << std::fixed << sz / 1024.0 << L" KB";
    else if(sz < 1024 * 1024 * 1024)ret << std::setprecision(2) << std::fixed << sz / 1024.0 / 1024.0 << L" MB";
    else if(sz < 1024ULL * 1024 * 1024 * 1024)ret << std::setprecision(2) << std::fixed << sz / 1024.0 / 1024.0 / 1024.0 << L" GB";
    else ret << std::setprecision(2) << std::fixed << sz / 1024.0 / 1024.0 / 1024.0 / 1024.0 << L" TB";

    return ret.str();
}

#define Bytes2KB(sz) ((double)(sz) / 1024)
#define Bytes2MB(sz) ((double)(sz) / 1024 / 1024)
#define Bytes2GB(sz) ((double)(sz) / 1024 / 1024 / 1024)

inline std::wstring s2ws(const std::string& str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

inline std::string ws2s(const std::wstring& wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}
}