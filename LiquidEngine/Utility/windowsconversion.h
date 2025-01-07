#pragma once

#include <string>
#include <Windows.h>
#include <algorithm>
#include <codecvt>
#include <locale>

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

static std::wstring string_to_wstring(const std::string& string) {
	std::wstring wsTmp(string.begin(), string.end());
	return wsTmp;
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
