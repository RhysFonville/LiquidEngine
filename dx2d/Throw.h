#pragma once

#include <exception>
#include <Windows.h>
#include <comdef.h>
#include <stacktrace>
#include "globalutil.h"

inline bool INFO_MESSAGE(std::string message) {
	int out = MessageBoxExA(NULL, message.c_str(), "Attention!", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1, 0);
	return false;
}

struct THROW_PARAMS {
	HRESULT hr;
	std::string message;
	std::string function_that_threw;
	std::string func_location;
	std::string file_location;
	long line_location;
	std::string extra_message = "";
};

inline bool ERROR_MESSAGE(THROW_PARAMS params) {
	std::string final_message = "+===== ERROR AT " + format_time_point(std::chrono::system_clock::now()) + " =====+";
	std::string print_final_message = params.message + " : " + params.extra_message + "\n\n" + params.function_that_threw;
	
	final_message += "\nERROR CODE " + std::to_string(params.hr) + ": " + params.message + (!params.extra_message.empty() ? " : " + params.extra_message : "") + '\n';
	final_message += "FUNCTION THAT THREW: " + params.function_that_threw + "\n\n";
	final_message += "LOCATION:\n";
	final_message += "\tFUNCTION: " + params.func_location + '\n';
	final_message += "\tFILE: " + params.file_location + '\n';
	final_message += "\tLINE: " + std::to_string(params.line_location) + "\n\n";
	final_message += "\n\nSTACK TRACE:\n" + std::to_string(std::stacktrace::current());
	append_to_file("throw_details.log", final_message + "\n\n");

	int out = MessageBoxExA(NULL, print_final_message.c_str(), "Error!", MB_CANCELTRYCONTINUE | MB_ICONERROR, 0);
	if (out == IDCANCEL) {
		throw std::exception(params.message.c_str());
		return false;
	} else if (out == IDTRYAGAIN) {
		return true;
	} else {
		return false;
	}
}

inline bool WARNING_MESSAGE(std::string message) {
	int out = MessageBoxExA(NULL, message.c_str(), "Warning!", MB_CANCELTRYCONTINUE | MB_ICONWARNING | MB_DEFBUTTON3, 0);
	if (out == IDABORT) {
		throw std::exception(message.c_str());
		return false;
	} else if (out == IDRETRY) {
		return true;
	} else {
		return false;
	}
}

inline bool YESNO_MESSAGE(std::string message) {
	int out;
	out = MessageBoxExA(NULL, message.c_str(), "Attention!", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1, 0);
	
	if (out == IDYES) {
		return true;
	} else {
		return false;
	}
}

//struct THROW_PARAMS {
//	HRESULT hr;
//	std::string message;
//	std::string function_that_threw;
//	std::string func_location;
//	std::string file_location;
//	size_t line_location;
//	std::string extra_message = "";
//};

inline bool CHECK_RESULT(THROW_PARAMS params) {
	if (FAILED(params.hr)) {
		_com_error error(params.hr);
		return ERROR_MESSAGE(THROW_PARAMS({
			error.Error(),
			wstring_to_string(error.ErrorMessage()),
			params.function_that_threw,
			params.func_location,
			params.file_location,
			params.line_location,
			params.extra_message
		}));
	} else {
		return false;
	}
}

static HRESULT hpewr = S_OK; // Handle Possible Excpetion (Windows) Result

// HPEW - Handle Possible Exception (Windows)
#define HPEW_1_ARG(function) \
hpewr = function; \
while (CHECK_RESULT(THROW_PARAMS({ hpewr, "", #function, __func__, __FILE__, __LINE__ })) == true) { \
	hpewr = function; \
}

#define HPEW_2_ARGS(function, extra_message) \
hpewr = function; \
while (CHECK_RESULT(THROW_PARAMS({ hpewr, "", #function, __func__, __FILE__, __LINE__, extra_message }))) == true) { \
	OutputDebugStringA(extra_message); \
	hpewr = function; \
}

static bool hper = false; // Handle Possible Excpetion Result

// HPE - Handle Possible Exception
#define HPE_1_ARG(function) \
do { \
	try { \
		function; \
	} catch (const std::exception &e) { \
		hper = ERROR_MESSAGE(THROW_PARAMS({ 0, e.what(), #function, __func__, __FILE__, __LINE__ })); \
	} \
} while (hper == true);

#define HPE_2_ARGS(function, extra_message) \
do { \
	try { \
		function; \
	} catch (const std::exception &e) { \
		hper = ERROR_MESSAGE(THROW_PARAMS({ 0, e.what(), #function, __func__, __FILE__, __LINE__, extra_message })); \
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
