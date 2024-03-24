#pragma once

#include <exception>
#include <Windows.h>
#include <comdef.h>
#include <stacktrace>
#include "../globalutil.h"
#include "DebugConsole.h"

static HRESULT hpewr = S_OK; // Handle Possible Excpetion (Windows) Result
static bool hpewquit = false;

/**
* Throw information for a HPEW catch.
*/
struct THROW_PARAMS {
	HRESULT hr;
	std::string message;
	std::string function_that_threw;
	std::string func_location;
	std::string file_location;
	long line_location;
	std::string extra_message = "";
};

/**
* Displays an error message and logs the information into throw_details.log and the console.
* \params params Throw information for the HPEW catch.
* \return True if retry was selected, false otherwise.
*/
static bool ERROR_MESSAGE(THROW_PARAMS params) {
	std::string file_message{};
	file_message += "+===== ERROR AT " + format_time_point(std::chrono::system_clock::now()) + " =====+";
	file_message += "\nERROR CODE " + std::to_string(params.hr) + ": " + params.message + (!params.extra_message.empty() ? " : " + params.extra_message : "") + '\n';
	file_message += "FUNCTION THAT THREW: " + params.function_that_threw + "\n\n";
	file_message += "LOCATION:\n";
	file_message += "\tFUNCTION: " + params.func_location + '\n';
	file_message += "\tFILE: " + params.file_location + '\n';
	file_message += "\tLINE: " + std::to_string(params.line_location);
	file_message += "\n\nSTACK TRACE:\n" + std::to_string(std::stacktrace::current());
	append_to_file(file_message + "\n\n", "throw_details.log");

	std::string print_message = params.message + " : " + params.extra_message + "\n\n" + params.function_that_threw;
	*debug_console << DebugConsole::Color::RED << "ERROR: " << print_message << '\n';

	int out = MessageBoxExA(NULL, print_message.c_str(), "Error!", MB_ABORTRETRYIGNORE | MB_ICONERROR, 0);
	if (out == IDABORT) {
		hpewquit = true;
		return false;
	} else if (out == IDRETRY) {
		return true;
	} else {
		return false;
	}
}

/**
* Displays an information popup.
* 
* \param message Message to announce.
* \return Returns the action that the user takes. See microsoft documentation for the return value of MessageBoxExA for uType = MB_OK.
*/
static bool INFO_MESSAGE(std::string message) {
	int out = MessageBoxExA(NULL, message.c_str(), "Attention!", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1, 0);
	return out;
}

/**
* Displays an warning popup.
* 
* \param message Message to warn about.
* \return True if retry was selected, false otherwise.
*/
static bool WARNING_MESSAGE(std::string message) {
	int out = MessageBoxExA(NULL, message.c_str(), "Warning!", MB_ABORTRETRYIGNORE | MB_ICONWARNING | MB_DEFBUTTON3, 0);
	if (out == IDABORT) {
		hpewquit = true;
		return false;
	} else if (out == IDRETRY) {
		return true;
	} else {
		return false;
	}
}

/**
* Displays a yes/no prompt.
* 
* \param message Message to ask about.
* \return Returns the action that the user takes. See microsoft documentation for the return value of MessageBoxExA for uType = MB_YESNO.
*/
static bool YESNO_MESSAGE(std::string message) {
	int out;
	out = MessageBoxExA(NULL, message.c_str(), "Attention!", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1, 0);
	
	if (out == IDYES) {
		return true;
	} else {
		return false;
	}
}

/**
 * Checks the result of the HPEW catch.
 * 
 * \params params Throw information for the HPEW catch.
 * \return Outcome of error message, false if nothing failed in the first place.
 */
static bool CHECK_RESULT(THROW_PARAMS params) {
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

// HPEW - Handle Possible Exception (Windows)
#define HPEW_1_ARG(function) \
hpewr = function; \
while (CHECK_RESULT(THROW_PARAMS({ hpewr, "", #function, __func__, __FILE__, __LINE__ }))) { \
	hpewr = function; \
} \
if (hpewquit) { \
	throw std::exception{}; \
}

#define HPEW_2_ARGS(function, extra_message) \
hpewr = function; \
while (CHECK_RESULT(THROW_PARAMS({ hpewr, "", #function, __func__, __FILE__, __LINE__, extra_message })))) { \
	OutputDebugStringA(extra_message); \
	hpewr = function; \
} \
if (hpewquit) { \
	throw std::exception{}; \
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
