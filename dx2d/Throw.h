#pragma once

#include <exception>
#include <Windows.h>
#include <comdef.h>
#include "Conversion.h"

inline bool ERROR_MESSAGE(std::string message, std::string extra_message = "") {
	int out = MessageBoxExA(NULL, (message + "\n\n" + extra_message).c_str(), "Error!", MB_CANCELTRYCONTINUE | MB_HELP | MB_ICONERROR | MB_DEFBUTTON1, 0);
	if (out == IDCANCEL) {
		throw std::exception((message + "\n\n" + extra_message).c_str());
		return false;
	} else if (out == IDTRYAGAIN) {
		return true;
	} else {
		return false;
	}
}

inline bool WARNING_MESSAGE(std::string message) {
	int out = MessageBoxExA(NULL, message.c_str(), "Warning!", MB_CANCELTRYCONTINUE | MB_HELP | MB_ICONWARNING | MB_DEFBUTTON3, 0);
	if (out == IDABORT) {
		throw std::exception(message.c_str());
		return false;
	} else if (out == IDRETRY) {
		return true;
	} else {
		return false;
	}
}

inline bool INFO_MESSAGE(std::string message) {
	int out = MessageBoxExA(NULL, message.c_str(), "Attention!", MB_OK | MB_HELP | MB_ICONINFORMATION | MB_DEFBUTTON1, 0);
	return false;
}

inline bool YESNO_MESSAGE(std::string message, bool help = true) {
	int out;
	if (help)
		out = MessageBoxExA(NULL, message.c_str(), "Attention!", MB_YESNO | MB_HELP | MB_ICONQUESTION | MB_DEFBUTTON1, 0);
	else
		out = MessageBoxExA(NULL, message.c_str(), "Attention!", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1, 0);
	
	if (out == IDYES) {
		return true;
	} else {
		return false;
	}
}

inline bool CHECK_RESULT(HRESULT hr, std::string extra_message = "") {
	if (FAILED(hr)) {
		_com_error error(hr);
		std::string str = "ERROR CODE " + std::to_string(error.Error()) + ": "
			+ std::string(wstring_to_string(error.ErrorMessage()));
		return ERROR_MESSAGE(str, extra_message);
	} else {
		return false;
	}
}

static HRESULT hpewr = S_OK; // Handle Possible Excpetion (Windows) Result

// HPEW - Handle Possible Exception (Windows)
#define HPEW_1_ARG(function) \
hpewr = function; \
while (CHECK_RESULT(hpewr, std::string("Function: ") + #function) == true) { \
	hpewr = function; \
}

#define HPEW_2_ARGS(function, extra_message) \
hpewr = function; \
while (CHECK_RESULT(hpewr, extra_message + std::string("Function: ") + #function) == true) { \
	hpewr = function; \
}

static bool hper = false; // Handle Possible Excpetion Result

// HPE - Handle Possible Exception
#define HPE_1_ARG(function) \
do { \
	try { \
		function; \
	} catch (std::exception &e) { \
		hper = ERROR_MESSAGE(e.what()); \
	} \
} while (hper == true);

#define HPE_2_ARGS(function, extra_message) \
do { \
	try { \
		function; \
	} catch (std::exception &e) { \
		hper = ERROR_MESSAGE(e.what(), extra_message); \
	} \
} while (hper == true);

#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3

#define HPEW_MACRO_CHOOSER(...) \
    GET_3RD_ARG(__VA_ARGS__, HPEW_2_ARGS, \
                HPEW_1_ARG, )

#define HPEW(...) HPEW_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define HPE_MACRO_CHOOSER(...) \
    GET_3RD_ARG(__VA_ARGS__, HPE_2_ARGS, \
                HPE_1_ARG, )

#define HPE(...) HPE_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
