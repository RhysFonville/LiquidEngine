#pragma once

#include <filesystem>
#include "../Components/CameraComponent.h"
#include "../Components/DirectionalLightComponent.h"
#include "../Components/PointLightComponent.h"
#include "../Components/SpotlightComponent.h"
#include "../Components/StaticMeshComponent.h"
#include "GraphicsPipeline.h"

#pragma comment(lib,"D3D12.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment(lib, "dxgi.lib")

static constexpr UINT MAX_LIGHTS_PER_TYPE = 16u;

_declspec(align(16))
struct DXDLData {
	DXDLData() { }
	DXDLData(const DirectionalLightComponent::DLData &data)
		: direction(data.direction),
		diffuse(ctofvec(data.diffuse)/255.0f),
		specular(ctofvec(data.specular)/255.0f) { }

	FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
	float pad = 0.0f;
	FVector4 diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	int null = false;

	FVector3 pad1 = FVector3(0.0f, 0.0f, 0.0f);
};

_declspec(align(16))
struct DXPLData {
	DXPLData() { }
	DXPLData(const PointLightComponent::PLData &data, const FVector3 &pos)
		: range(data.range), attenuation(data.attenuation),
		diffuse(ctofvec(data.diffuse)/255.0f), 
		specular(ctofvec(data.specular)/255.0f),
		position(pos) { }


	float range = 100.0f;
	FVector3 attenuation = FVector3(0.2f, 0.2f, 0.2f);
	FVector4 diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	int null = false;

	FVector3 position = FVector3(0.0f, 0.0f, 0.0f);
};

_declspec(align(16))
struct DXSLData {
	DXSLData() { }
	DXSLData(const SpotlightComponent::SLData &data)
		: direction(data.direction),
		diffuse(ctofvec(data.diffuse)/255.0f),
		specular(ctofvec(data.specular)/255.0f) { }


	FVector3 direction = FVector3(0.0f, 0.0f, 0.0f);
	FVector4 diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	int null = 0;

	FVector3 pad = FVector3(0.0f, 0.0f, 0.0f);;
};

_declspec(align(16))
struct DXMatData {
	DXMatData() { }
	DXMatData(const Material::MaterialData &data)
		: a(data.a), ks(color_to_fvector(data.ks)/255.0f),
		kd(color_to_fvector(data.kd)/255.0f),
		ka(color_to_fvector(data.ka)/255.0f) { }

	FVector4 ks = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // Specular
	FVector4 kd = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // Diffuse
	FVector4 ka = FVector4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient
	float a = 0.5f; // Shininess
};

_declspec(align(256))
struct PerFrameVSCB { // b0
	XMMATRIX WVP;
};

//_declspec(align(256))
struct PerObjectVSCB { // b1
	XMMATRIX transform;
};

//_declspec(align(256))
struct PerFramePSCB { // b2
	FVector3 camera_position;

	UINT directional_light_count = 0;
	UINT point_light_count = 0;
	UINT spotlight_count = 0;

	DXDLData directional_lights[MAX_LIGHTS_PER_TYPE] = { };
	DXPLData point_lights[MAX_LIGHTS_PER_TYPE] = { };
	DXSLData spotlights[MAX_LIGHTS_PER_TYPE] = { };
};

//__declspec(align(256))
struct PerObjectPSCB { // b3
	DXMatData material;
};

class GraphicsScene {
public:
	GraphicsScene() { }
	GraphicsScene(HWND window, const std::vector<StaticMeshComponent*> &static_meshes); // initializes direct3d 12

	void tick(); // Updates pipeline and renders
	void clean_up(); // release com ojects and clean up memory

	void update(); // update the direct3d pipeline (update command lists)
	void render(); // execute the command list
	void increment_fence(); // increment fences
	void wait_for_fence_cpu(); // wait for fences incrementation on cpu side
	void wait_for_fence_gpu(); // wait for fences incrementation on gpu side
	void wait_for_previous_frame(); // wait for command list completion

	void compile();

	void set_fullscreen(bool fullscreen);
	void toggle_fullscreen();
	GET bool is_fullscreen() const noexcept;

	GET UVector2 get_resolution() const noexcept;
	void set_resolution(const UVector2 &resolution, bool reset_om_viewing_settings = true);

	std::vector<StaticMeshComponent*> static_meshes;
	CameraComponent* camera;
	std::vector<Component*> lights;

private:
	friend class Window;

	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	ComPtr<IDXGISwapChain3> swap_chain = nullptr;
	ComPtr<ID3D12CommandAllocator> command_allocators[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;
	ComPtr<ID3D12Fence> fences[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	HANDLE fences_event = nullptr;
	ULONGLONG fence_values[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap = nullptr;
	UINT rtv_descriptor_size = 0u; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
	ComPtr<ID3D12Resource> render_targets[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	unsigned int buffer_index = 0u;
	ComPtr<IDXGIFactory4> factory = nullptr;
	std::string video_card_desc;
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	ComPtr<IDXGIOutput> adapter_output = nullptr;

	ComPtr<ID3D12Debug> debug_interface = nullptr;
	ComPtr<ID3D12DebugDevice> debug_device = nullptr;
	ComPtr<ID3D12DebugCommandList> debug_command_list = nullptr;
	ComPtr<ID3D12DebugCommandQueue> debug_command_queue = nullptr;

	void create_adapter_and_device();
	void create_command_queue();
	void create_swap_chain();
	void create_back_buffers_and_rtv_with_descriptor_heap();
	void create_command_allocators();
	void create_command_list();
	void create_fences_and_fences_event();

	struct CBS {
		PerFrameVSCB per_frame_vs = PerFrameVSCB();
		PerObjectVSCB per_object_vs = PerObjectVSCB();
		GraphicsPipeline::ConstantBuffer<PerFramePSCB> per_frame_ps = GraphicsPipeline::ConstantBuffer<PerFramePSCB>(PerFramePSCB());
		GraphicsPipeline::ConstantBuffer<PerObjectPSCB> per_object_ps = GraphicsPipeline::ConstantBuffer<PerObjectPSCB>(PerObjectPSCB());
	} cbs;

	UINT frame_index = 0u;

	DXGI_SAMPLE_DESC sample_desc = { };
	DXGI_MODE_DESC back_buffer_desc = { }; // this is to describe our display mode

	HWND window = nullptr;

	bool fullscreen = false;

	UVector2 resolution = UVector2(3840, 2160);

	FColor background_color = { 0.25f, 0.25f, 0.25f, 1.0f };
};
