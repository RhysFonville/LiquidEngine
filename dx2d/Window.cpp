#include "Window.h"

Window *this_window_wndproc;

LRESULT Window::wndproc(HWND hwnd, UINT32 uMsg, WPARAM wParam, LPARAM lParam) {
	if (this_window_wndproc != nullptr) {
		switch (uMsg) {
			case WM_CLOSE:
				if (MessageBoxExW(
						*this_window_wndproc->get_window(),
						L"Are you sure you want to quit?",
						L"Exit",
						MB_YESNO | MB_ICONWARNING, 0
					) == IDYES) {
					DestroyWindow(*this_window_wndproc->get_window());

					this_window_wndproc->running = false;

					this_window_wndproc = NULL;

					return 0;
				}
				return 1;
			case WM_DESTROY:
				PostQuitMessage(WM_QUIT);
				break;
			case WM_SIZE:
				RECT rect;
				if (GetWindowRect(hwnd, &rect)) {
					UINT width = rect.right - rect.left;
					UINT height = rect.bottom - rect.top;
					this_window_wndproc->size = Size(width, height);
				}

				/*this_window_wndproc->graphics_scene->swap_chain->ResizeBuffers(1u,
					this_window_wndproc->size.width,
					this_window_wndproc->size.height,
					DXGI_FORMAT_B8G8R8A8_UNORM,
					0u);*/

				break;
		}
	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

Window::Window(WNDPROC window_procedure, HINSTANCE hInstance) {
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Window Class";
	window_class.lpfnWndProc = window_procedure;

	this->hInstance = hInstance;

	RegisterClassW(&window_class);
}

Window::Window(HINSTANCE hInstance) {

	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Window Class";
	window_class.lpfnWndProc = wndproc;

	this->hInstance = hInstance;

	RegisterClassW(&window_class);
}

Window::Window(HINSTANCE hInstance, std::shared_ptr<GraphicsScene> graphics_scene)
	: graphics_scene(graphics_scene) {

	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Window Class";
	window_class.lpfnWndProc = wndproc;

	this->hInstance = hInstance;

	RegisterClassW(&window_class);
}

void Window::set_up_window(const Position &position, const Size &size, const std::string &name, DWORD style, const HWND &parent, DWORD extended_style, HMENU menu, void *lpParam) {
	HWND hwnd = CreateWindowExW(extended_style, window_class.lpszClassName, string_to_wstring(name).c_str(),
		style, position.x, position.y,
		800, 600, parent, menu, hInstance, lpParam);

	window = std::make_shared<HWND>(hwnd);
	dc = GetDC(*window);
}

void Window::check_input() {
	MSG message = { };
	while (PeekMessageW(&message, *window, 0, 0, PM_REMOVE)) {
		this_window_wndproc = this;

		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}

std::shared_ptr<HWND> Window::get_window() {
	return window;
}

HDC & Window::get_dc() {
	return dc;
}

HINSTANCE Window::get_hInstance() {
	return hInstance;
}

WNDCLASS & Window::get_class() {
	return window_class;
}

Size Window::get_size() const noexcept {
	return size;
}

void Window::set_size(const Size &size) noexcept {
	this->size = size;
	SetWindowPos(*window, 0, 0, 0, size.width, size.height, SWP_SHOWWINDOW | SWP_NOMOVE);
}

bool Window::is_running() const noexcept {
	return running;
}

void Window::is_running(bool running) noexcept {
	this->running = running;
}
