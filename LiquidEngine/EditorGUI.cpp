#include "EditorGUI.h"

EditorGUI::EditorGUI() { }
EditorGUI::EditorGUI(HWND hwnd) : hwnd{hwnd} { }

void EditorGUI::init_with_renderer(HWND hwnd, ID3D12Device* device, int num_buffers,
	ID3D12DescriptorHeap* cbv_srv_heap) {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplWin32_Init((void*)hwnd);
	ImGui::StyleColorsDark();
	
	ImGui_ImplDX12_Init(device, num_buffers, DXGI_FORMAT_R8G8B8A8_UNORM, cbv_srv_heap,
		cbv_srv_heap->GetCPUDescriptorHandleForHeapStart(), cbv_srv_heap->GetGPUDescriptorHandleForHeapStart());
}

bool EditorGUI::check_input(UINT32 message, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam)) {
		return true;
	}
	return false;
}

void EditorGUI::update(float dt, const std::vector<std::shared_ptr<Object>> &objects) {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Editor");

	ImGui::Text("FPS: %.3f(DT: %.3f)", 1000.0f / dt, dt);
	
	if (ImGui::Button("Show demo button"))
		show_demo_window = !show_demo_window;

	if (show_demo_window) {
		ImGui::ShowDemoWindow();
	}

	int i = 0;
	for (auto &object : objects) {
		if (ImGui::TreeNode((std::string{"Object #"} + std::to_string(i)).c_str())) {
			ImGui::Text("Transform");
			float vec[3]{object->get_position().x, object->get_position().y, object->get_position().z};
			if (ImGui::InputFloat3("Position", vec))
				object->set_position(vec);
			vec[0] = object->get_rotation().x;
			vec[1] = object->get_rotation().y;
			vec[2] = object->get_rotation().z;
			if (ImGui::InputFloat3("Rotation", vec))
				object->set_rotation(vec);
			vec[0] = object->get_size().x;
			vec[1] = object->get_size().y;
			vec[2] = object->get_size().z;
			if (ImGui::InputFloat3("Size", vec))
				object->set_size(vec);
			
			ImGui::Text("Components");
			for (std::shared_ptr<Component> &component : object->get_all_components()) {
				if (ImGui::TreeNode(typeid(component).name())) {
					component->base_render_editor_gui_section();
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
		i++;
	}

	ImGui::End();

	ImGui::Render();
}

void EditorGUI::clean_up() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
