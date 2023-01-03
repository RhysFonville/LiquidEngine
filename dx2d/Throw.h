#pragma once

#include <exception>
#include <Windows.h>
#include <comdef.h>
#include "Conversion.h"

inline bool ERROR_MESSAGE(std::wstring message) {
	int out = MessageBoxExW(NULL, message.c_str(), L"Error!", MB_CANCELTRYCONTINUE | MB_HELP | MB_ICONERROR | MB_DEFBUTTON1, 0);
	if (out == IDCANCEL) {
		throw std::exception(wstring_to_string(message).c_str());
		return false;
	} else if (out == IDTRYAGAIN) {
		return true;
	} else {
		return false;
	}
}

inline bool WARNING_MESSAGE(std::wstring message) {
	int out = MessageBoxExW(NULL, message.c_str(), L"Warning!", MB_CANCELTRYCONTINUE | MB_HELP | MB_ICONWARNING | MB_DEFBUTTON3, 0);
	if (out == IDABORT) {
		throw std::exception(wstring_to_string(message).c_str());
		return false;
	} else if (out == IDRETRY) {
		return true;
	} else {
		return false;
	}
}

inline bool INFO_MESSAGE(std::wstring message) {
	int out = MessageBoxExW(NULL, message.c_str(), L"Attention!", MB_OK | MB_HELP | MB_ICONINFORMATION | MB_DEFBUTTON1, 0);
	return false;
}

inline bool YESNO_MESSAGE(std::wstring message, bool help = true) {
	int out;
	if (help)
		out = MessageBoxExW(NULL, message.c_str(), L"Attention!", MB_YESNO | MB_HELP | MB_ICONQUESTION | MB_DEFBUTTON1, 0);
	else
		out = MessageBoxExW(NULL, message.c_str(), L"Attention!", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1, 0);
	
	if (out == IDYES) {
		return true;
	} else {
		return false;
	}
}

inline bool CHECK_RESULT(HRESULT hr) {
	if (FAILED(hr)) {
		_com_error error(hr);
		std::wstring str = L"ERROR CODE " + std::to_wstring(error.Error()) + L": "
			+ std::wstring(error.ErrorMessage());
		return ERROR_MESSAGE(str);
	} else {
		return false;
	}
}

static HRESULT hpewr = S_OK; // Handle Possible Excpetion (Windows) Result
// HPEW - Handle Possible Exception (Windows)
#define HPEW(function) \
hpewr = function; \
while (CHECK_RESULT(hpewr) == true) { \
	hpewr = function; \
}

static bool hper = false; // Handle Possible Excpetion Result
// HPE - Handle Possible Exception
#define HPE(function) \
do { \
	try { \
		function; \
	} catch (std::exception &e) { \
		hper = ERROR_MESSAGE(string_to_wstring(e.what())); \
	} \
} while (hper == true);
