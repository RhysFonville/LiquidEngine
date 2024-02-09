#include "DebugConsole.h"

DebugConsole* DebugConsole::debug_console = nullptr;

DebugConsole::DebugConsole() {
	std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	hwnd = GetConsoleWindow();
}

DebugConsole* DebugConsole::get_instance() {
	if (debug_console == nullptr) {
		debug_console = new DebugConsole{};
	}
	return debug_console;
}

DebugConsole & DebugConsole::operator<<(char out) noexcept {
	std::cout << out;
	return *this;
}

DebugConsole & DebugConsole::operator<<(const std::string &out) noexcept {
	std::cout << out;
	return *this;
}

DebugConsole & DebugConsole::operator>>(std::string &in) noexcept {
	std::cin >> in;
	return *this;
}

DebugConsole & DebugConsole::operator<<(const Color &col) noexcept {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)col);
	return *this;
}
