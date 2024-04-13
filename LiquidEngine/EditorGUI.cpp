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

OpenFileRet EditorGUI::open_file() {
	std::string sSelectedFile;
	std::string sFilePath;

	//  CREATE FILE OBJECT INSTANCE
	HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(f_SysHr))
		return {false, sSelectedFile, sFilePath};

	// CREATE FileOpenDialog OBJECT
	IFileOpenDialog* f_FileSystem;
	f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
	if (FAILED(f_SysHr)) {
		CoUninitialize();
		return {false, sSelectedFile, sFilePath};
	}

	//  SHOW OPEN FILE DIALOG WINDOW
	f_SysHr = f_FileSystem->Show(NULL);
	if (FAILED(f_SysHr)) {
		f_FileSystem->Release();
		CoUninitialize();
		return {false, sSelectedFile, sFilePath};
	}

	//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
	IShellItem* f_Files;
	f_SysHr = f_FileSystem->GetResult(&f_Files);
	if (FAILED(f_SysHr)) {
		f_FileSystem->Release();
		CoUninitialize();
		return {false, sSelectedFile, sFilePath};
	}

	//  STORE AND CONVERT THE FILE NAME
	PWSTR f_Path;
	f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
	if (FAILED(f_SysHr)) {
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return {false, sSelectedFile, sFilePath};
	}

	sFilePath = wstring_to_string(std::wstring{f_Path});

	//  FORMAT STRING FOR EXECUTABLE NAME
	const size_t slash = sFilePath.find_last_of("/\\");
	sSelectedFile = sFilePath.substr(slash + 1);

	//  SUCCESS, CLEAN UP
	CoTaskMemFree(f_Path);
	f_Files->Release();
	f_FileSystem->Release();
	CoUninitialize();
	return {true, sSelectedFile, sFilePath};
}
