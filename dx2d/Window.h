#pragma once
#ifndef RWINDOW_H
#define RWINDOW_H

#include <string>
#include <functional>
#include "GraphicsScene.h"
#include "globalstructs.h"

class Window {
public:
	Window(WNDPROC window_procedure, HINSTANCE hInstance);
	Window(HINSTANCE hInstance);
	Window(HINSTANCE hInstance, std::shared_ptr<GraphicsScene> graphics_scene);

	void set_up_window(const Vector2 &position = Vector2(CW_USEDEFAULT, CW_USEDEFAULT),
		const Vector2 &size = Vector2(1200, 800),
		const std::string &name = "Epic Window :coolglasses:",
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE, const HWND &parent = NULL,
		DWORD extended_style = NULL, HMENU menu = NULL, void* lpParam = NULL);

	void check_input();

	void clean_up();

	GET HWND & get_window() noexcept;
	GET HDC & get_dc() noexcept;
	GET HINSTANCE & get_hInstance() noexcept;
	GET WNDCLASS & get_class() noexcept;

	GET UVector2 get_size() const noexcept;
	void set_size(const UVector2 &size) noexcept;

	GET bool is_running() const noexcept;
	void is_running(bool running) noexcept;

	GraphicsScene* graphics_scene;

private:
	HWND window;
	HDC dc;
	HINSTANCE hInstance;
	WNDCLASS window_class = { };
	
	UVector2 size;

	bool running = true;

	static LRESULT CALLBACK wndproc(HWND hwnd, UINT32 uMsg, WPARAM wParam, LPARAM lParam);
	
	bool first_size = true;
};

#endif
