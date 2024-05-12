#pragma once
#ifndef RWINDOW_H
#define RWINDOW_H

#include <string>
#include <functional>
#include "Graphics/Renderer/Renderer.h"
#include "Utility/globalstructs.h"

#ifndef NDEBUG // Not debug (so release, idek), so when this statement is true, we in release
#include "Debug/DebugConsole.h"
#include "EditorGUI.h"
#endif

/**
 * The main window for rendering.
 * \see GraphicsScene
 */

class Window {
public:
	Window();
	Window(HINSTANCE hInstance);
	Window(HINSTANCE hInstance, Renderer* graphics_scene);
	Window(Renderer* renderer);

	/**
	 * Creates and sets up the window.
	 * 
	 * \param position Initial position of window.
	 * \param size Initial size of window.
	 * \param name Window title.
	 * \param style Window styles.
	 * \param parent Parent window.
	 * \param extended_style More specific window styles.
	 * \param menu Window menu. Usually unneeded.
	 * \param lpParam Unneeded.
	 */
	void set_up_window(const Vector2 &position = Vector2(CW_USEDEFAULT, CW_USEDEFAULT),
		const Vector2 &size = Vector2(1200, 800),
		const std::string &name = "Epic Window :coolglasses:",
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE, const HWND &parent = NULL,
		DWORD extended_style = NULL, HMENU menu = NULL, void* lpParam = NULL);

	void check_input();

	void clean_up();

	GET HWND & get_window() noexcept;
	GET HDC & get_dc() noexcept;
	GET WNDCLASSEXW & get_class() noexcept;

	GET UVector2 get_size() const noexcept;
	void set_size(const UVector2 &size) noexcept;

	GET bool is_running() const noexcept;
	void is_running(bool running) noexcept;

	Renderer* graphics_scene = nullptr;

	EditorGUI editor_gui{};

private:
	HWND window = NULL;
	HDC dc = NULL;
	WNDCLASSEXW window_class = { };
	
	UVector2 size{};

	bool running = true;

	static LRESULT CALLBACK wndproc(HWND hwnd, UINT32 uMsg, WPARAM wParam, LPARAM lParam);
};

#endif
