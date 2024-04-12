#include "EditorGUI.h"

void EditorGUI::init_with_renderer(HWND hwnd, ID3D12Device* device, int num_buffers,
	ID3D12DescriptorHeap* cbv_srv_heap) {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init((void*)hwnd);
	ImGui_ImplDX12_Init(device, num_buffers, DXGI_FORMAT_R8G8B8A8_UNORM, cbv_srv_heap,
		cbv_srv_heap->GetCPUDescriptorHandleForHeapStart(),
		cbv_srv_heap->GetGPUDescriptorHandleForHeapStart());
}

bool EditorGUI::check_input(HWND hwnd, UINT32 message, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam)) {
		return true;
	}
	return false;
}

void EditorGUI::update(float dt) {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Editor");

	ImGui::Text("FPS: %.3f(DT: %.3f)", 1000.0f / dt, dt);

	if (ImGui::Button("Show demo button"))
		show_demo_window = !show_demo_window;

	if (show_demo_window) {
		ImGui::ShowDemoWindow();
		ImPlot::ShowDemoWindow();
	}
}

void EditorGUI::clean_up() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ImPlot::DestroyContext();
}
