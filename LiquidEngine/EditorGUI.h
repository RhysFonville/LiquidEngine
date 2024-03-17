#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <vector>
#include <memory>
#include <typeinfo>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include "Object.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);

/**
* ImGUI Editor GUI.
*/
class EditorGUI {
public:
	EditorGUI(EditorGUI &) = delete;
	void operator=(const EditorGUI &) = delete;

	static void init_with_renderer(HWND hwnd, ID3D12Device* device, int num_buffers, ID3D12DescriptorHeap* cbv_srv_heap);

	bool check_input(UINT32 message, WPARAM wparam, LPARAM lparam);

	void update(float dt, const std::vector<std::shared_ptr<Object>> &objects);
	void clean_up();

	HWND hwnd;

private:
	friend class Window;

	EditorGUI();
	EditorGUI(HWND hwnd);

	bool show_demo_window = true;
};
