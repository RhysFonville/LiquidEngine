#pragma once
#ifndef RWINDOW_H
#define RWINDOW_H

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "Conversion.h"
#include "GraphicsScene.h"

struct Position {
	int x;
	int y;

	Position(int x, int y) : x(x), y(y) {  }
};

struct Size {
	UINT width;
	UINT height;

	Size() : width(0u), height(0u) {}
	Size(UINT width, UINT height) : width(width), height(height) {  }
};

class Window {
public:
	Window(WNDPROC window_procedure, HINSTANCE hInstance);
	Window(HINSTANCE hInstance);
	Window(HINSTANCE hInstance, std::shared_ptr<GraphicsScene> graphics_scene);

	void set_up_window(const Position &position = Position(CW_USEDEFAULT, CW_USEDEFAULT),
		const Size &size = Size(800, 600),
		const std::string &name = "My Window",
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE, const HWND &parent = NULL,
		DWORD extended_style = NULL, HMENU menu = NULL, void* lpParam = NULL);

	void check_input();

	std::shared_ptr<HWND> get_window();
	HDC & get_dc();
	HINSTANCE get_hInstance();
	WNDCLASS & get_class();

	Size get_size() const noexcept;
	void set_size(const Size &size) noexcept;

	bool is_running() const noexcept;
	void is_running(bool running) noexcept;

	std::shared_ptr<GraphicsScene> graphics_scene;

private:
	std::shared_ptr<HWND> window;
	HDC dc;
	HINSTANCE hInstance;
	WNDCLASS window_class = { };
	
	Size size;

	bool running = true;

	static LRESULT CALLBACK wndproc(HWND hwnd, UINT32 uMsg, WPARAM wParam, LPARAM lParam);
};

#endif
