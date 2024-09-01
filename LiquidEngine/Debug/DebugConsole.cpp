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

void DebugConsole::destroy_window() {
	DestroyWindow(hwnd);
}

void DebugConsole::clear_old_history(int number_of_logs) {
	if (number_of_logs == -1) {
		history.clear();
	} else {
		history.erase(history.begin(), history.begin()+number_of_logs+1);
	}
}

DebugConsole & DebugConsole::operator<<(char out) noexcept {
	*this << std::string{1, out};
	return *this;
}

DebugConsole & DebugConsole::operator<<(const std::string &out) noexcept {
	std::cout << out;
	history.push_back(out);
	return *this;
}

DebugConsole & DebugConsole::operator<<(const Color &col) noexcept {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)col);
	return *this;
}

DebugConsole & DebugConsole::operator>>(std::string &in) noexcept {
	std::cin >> in;
	return *this;
}
