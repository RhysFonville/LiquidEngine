#pragma once

#pragma once
#ifndef CONVERSION_H
#define CONVERSION_H

#include <Windows.h>
#include <string>
#include <codecvt>
#include <algorithm>

static std::wstring string_to_wstring(const std::string& string, bool is_utf8 = true) {
    int len;
    int slength = (int)string.length() + 1;
    len = MultiByteToWideChar(is_utf8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), slength, 0, 0);
    std::wstring buf;
    buf.resize(len);
    MultiByteToWideChar(is_utf8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), slength,
        const_cast<wchar_t*>(buf.c_str()), len);
    return buf;
}

static std::string wstring_to_string(const std::wstring& wstring) {
    std::string str;
    str.resize(wstring.size());
    std::transform(wstring.begin(), wstring.end(), str.begin(),
        [](auto wc) {return static_cast<char>(wc); }
    );
    return str;
}

static std::wstring BSTR_to_wstring(BSTR bstr) {
    return std::wstring(bstr, SysStringLen(bstr));
}

#endif

