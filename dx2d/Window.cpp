#include "Window.h"

#define GRAPHICS_SCENE this_window_wndproc->graphics_scene

Window *this_window_wndproc;

LRESULT Window::wndproc(HWND hwnd, UINT32 uMsg, WPARAM wParam, LPARAM lParam) {
	if (this_window_wndproc != nullptr) {
		switch (uMsg) {
			case WM_CLOSE:
				if (YESNO_MESSAGE(L"Are you sure you want to exit?", false) == true)
					this_window_wndproc->running = false;
				return 0;
				break;
			case WM_DESTROY:
				PostQuitMessage(WM_QUIT);
				break;
			case WM_QUIT:
				break;
			case WM_SIZE:
				RECT rect;
				if (GetWindowRect(hwnd, &rect)) {
					UINT width = rect.right - rect.left;
					UINT height = rect.bottom - rect.top;
					this_window_wndproc->size = UVector2(width, height);
				}

				/*if (!this_window_wndproc->first_size) {
					if (GRAPHICS_SCENE) {
						GRAPHICS_SCENE->context->OMSetRenderTargets(0, 0, 0);

						// Release all outstanding references to the swap chain's buffers.
						GRAPHICS_SCENE->clean_up(false, false, false);

						// Preserve the existing buffer count and format.
						// Automatically choose the width and height to match the client rect for HWNDs.
						HANDLE_POSSIBLE_EXCEPTION_WINDOWS(GRAPHICS_SCENE->swap_chain->ResizeBuffers(0, this_window_wndproc->size.x,
							this_window_wndproc->size.y, DXGI_FORMAT_UNKNOWN, 0));
											
						// Perform error handling here!

						// Get buffer and create a render-target-view.
						ID3D11Texture2D* pBuffer;
						HANDLE_POSSIBLE_EXCEPTION_WINDOWS(GRAPHICS_SCENE->swap_chain->GetBuffer(
							0, 
							__uuidof(ID3D11Texture2D),
							(void**)&pBuffer)
						);
						// Perform error handling here!

						Microsoft::WRL::ComPtr<ID3D11RenderTargetView> target = nullptr;
						if (pBuffer != NULL) {
							HANDLE_POSSIBLE_EXCEPTION_WINDOWS(GRAPHICS_SCENE->device->CreateRenderTargetView(
								pBuffer,
								NULL,
								&target
							));
						} else {
							ERROR_MESSAGE(L"Swap chain render target resource ID3D11Resource is null when attempting to resize window.");
						}

						target.CopyTo(GRAPHICS_SCENE->target.GetAddressOf());
						target.Detach();
						GRAPHICS_SCENE->context->OMSetRenderTargets(1u, GRAPHICS_SCENE->target.GetAddressOf(), NULL);

						GRAPHICS_SCENE->create_unessentials();
					
						for (std::shared_ptr<Object> &object : *GRAPHICS_SCENE->objects) {
							std::shared_ptr<MeshComponent> mesh = GRAPHICS_SCENE->obj_mesh(*object);
							if (mesh != nullptr) {
								Material &material = mesh->material;
								material.compile(false);
							}
						}

						GRAPHICS_SCENE->compile();

						GRAPHICS_SCENE->context->IASetPrimitiveTopology(GRAPHICS_SCENE->primitive_topology);
					}
				}*/
				
				this_window_wndproc->first_size = false;
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

void Window::set_up_window(const Vector2 &position, const Vector2 &size, const std::string &name, DWORD style, const HWND &parent, DWORD extended_style, HMENU menu, void *lpParam) {
	window = CreateWindowExW(extended_style, window_class.lpszClassName, string_to_wstring(name).c_str(),
		style, position.x, position.y,
		size.x, size.y, parent, menu, hInstance, lpParam);
	dc = GetDC(window);
}

void Window::check_input() {
	MSG message = { };
	while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) {
		this_window_wndproc = this;

		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}

void Window::clean_up() {
	if (ReleaseDC(window, dc) == 0) {
		dc = GetDC(window);
		throw std::exception("Failed to release the device context.");
		ReleaseDC(window, dc);
	}
	if (DestroyWindow(this_window_wndproc->get_window()) == 0) {
		dc = GetDC(window);
		throw std::exception("Failed to destroy window.");
		ReleaseDC(window, dc);
	}
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

WNDCLASS & Window::get_class() noexcept {
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
