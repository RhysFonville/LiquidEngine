#pragma once

#include <exception>
#include <Windows.h>
#include <comdef.h>
#include "Conversion.h"

// Helper utility converts D3D API failures into exceptions.
inline bool CHECK_RESULT(HRESULT hr) {
	if (FAILED(hr)) {
		_com_error error(hr);
		std::wstring str = L"ERROR CODE " + std::to_wstring(error.Error()) + L": "
			+ std::wstring(error.ErrorMessage()) + L"\nHRESULTTOWCODE: "
			+ std::to_wstring(error.HRESULTToWCode(hr));
		
		int out = MessageBoxExW(NULL, str.c_str(), L"Error!", MB_ABORTRETRYIGNORE, 0);
		if (out == IDABORT) {
			exit(hr);
			return false;
		} else if (out == IDRETRY) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
static HRESULT r = S_OK;
#define HANDLE_POSSIBLE_EXCEPTION(function) \
r = function; \
while(CHECK_RESULT(r) == true) { \
	r = function; \
}
