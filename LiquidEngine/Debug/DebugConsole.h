#pragma once

#include <fstream>
#include <Windows.h>
#include <iostream>

/**
 * Debug window input and output.
 */
class DebugConsole {
public:
	enum class Color { // https://stackoverflow.com/questions/17125440/c-win32-console-color
		BLACK			= 0,
		DARKBLUE		= FOREGROUND_BLUE,
		DARKGREEN		= FOREGROUND_GREEN,
		DARKCYAN		= FOREGROUND_GREEN | FOREGROUND_BLUE,
		DARKRED			= FOREGROUND_RED,
		DARKMAGENTA		= FOREGROUND_RED | FOREGROUND_BLUE,
		DARKYELLOW		= FOREGROUND_RED | FOREGROUND_GREEN,
		DARKGRAY		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		GRAY			= FOREGROUND_INTENSITY,
		BLUE			= FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		GREEN			= FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		CYAN			= FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		RED				= FOREGROUND_INTENSITY | FOREGROUND_RED,
		MAGENTA			= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		YELLOW			= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		WHITE			= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		
		BG_NAVYBLUE = 16,
		BG_GREEN = 32,
		BG_TEAL = 48,
		BG_MAROON = 64,
		BG_PURPLE = 80,
		BG_OLIVE = 96,
		BG_SILVER = 112,
		BG_GRAY = 128,
		BG_BLUE = 144,
		BG_LIME = 160,
		BG_CYAN = 176,
		BG_RED = 192,
		BG_MAGENTA = 208,
		BG_YELLOW = 224,
		BG_WHITE = 240
	};


	DebugConsole(DebugConsole &) = delete;
	void operator=(const DebugConsole &) = delete;

	static DebugConsole* get_instance();

	DebugConsole & operator<<(char out) noexcept;
	DebugConsole & operator<<(const std::string &out) noexcept;
	DebugConsole & operator<<(const Color &col) noexcept;
	DebugConsole & operator>>(std::string &in) noexcept;

	HANDLE std_handle;
	HWND hwnd;

private:
	DebugConsole();

	static DebugConsole* debug_console;
};

static DebugConsole* debug_console = DebugConsole::get_instance();
