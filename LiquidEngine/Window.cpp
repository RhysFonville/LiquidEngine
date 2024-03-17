#include "Window.h"

#define GRAPHICS_SCENE this_window_wndproc->graphics_scene

Window* this_window_wndproc;

LRESULT Window::wndproc(HWND hwnd, UINT32 uMsg, WPARAM wParam, LPARAM lParam) {
	if (this_window_wndproc != nullptr) {
		if (this_window_wndproc->editor_gui.check_input(uMsg, wParam, lParam)) return LRESULT{};

		switch (uMsg) {
			case WM_CLOSE:
				if (YESNO_MESSAGE("Are you sure you want to exit?")) {
					this_window_wndproc->running = false;
				}
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_SIZE:
				RECT rect;
				if (GetWindowRect(hwnd, &rect)) {
					UINT width = rect.right - rect.left;
					UINT height = rect.bottom - rect.top;
					this_window_wndproc->size = UVector2(width, height);
				}
				
				this_window_wndproc->first_size = false;
				break;
			default:
				return DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}
	} else {
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	return LRESULT{};
}

Window::Window() {
	window_class.cbSize = sizeof(WNDCLASSEXW);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Window Class";
	window_class.lpfnWndProc = wndproc;
	window_class.hCursor = LoadCursorW(NULL, IDC_CROSS);

	RegisterClassExW(&window_class);
}

Window::Window(HINSTANCE hInstance) : Window{} {
	this->hInstance = hInstance;
}

Window::Window(HINSTANCE hInstance, Renderer* graphics_scene)
	: Window{hInstance} {
	this->graphics_scene = graphics_scene;
}

void Window::set_up_window(const Vector2 &position, const Vector2 &size, const std::string &name, DWORD style, const HWND &parent, DWORD extended_style, HMENU menu, void *lpParam) {
	window = CreateWindowExW(extended_style, window_class.lpszClassName, string_to_wstring(name).c_str(),
		style, position.x, position.y,
		size.x, size.y, parent, menu, hInstance, lpParam);

	editor_gui.hwnd = window;

	if (window == NULL) {
		throw std::exception("Failed to create window.");
	}

	dc = GetDC(window);
}

void Window::check_input() {
	MSG message = { };
	while (PeekMessageA(&message, window, 0, 0, PM_REMOVE)) {
		this_window_wndproc = this;

		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

void Window::clean_up() {
	HPEW(ReleaseDC(window, dc));
	HPEW(DestroyWindow(this_window_wndproc->get_window()));
#ifndef NDEBUG
	debug_console->destroy_window();
#endif // !NDEBUG

	this_window_wndproc = NULL;
	graphics_scene = nullptr;
}

HWND & Window::get_window() noexcept {
	return window;
}

HDC & Window::get_dc() noexcept {
	return dc;
}

HINSTANCE & Window::get_hInstance() noexcept {
	return hInstance;
}

WNDCLASSEXW & Window::get_class() noexcept {
	return window_class;
}

UVector2 Window::get_size() const noexcept {
	return size;
}

void Window::set_size(const UVector2 &size) noexcept {
	this->size = size;
	SetWindowPos(window, 0, 0, 0, size.x, size.y, SWP_SHOWWINDOW | SWP_NOMOVE);
}

bool Window::is_running() const noexcept {
	return running;
}

void Window::is_running(bool running) noexcept {
	this->running = running;
}
