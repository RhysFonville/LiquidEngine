#pragma once

#include <DirectXMath.h>
#include <filesystem>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "Object.h"
#include "globalutil.h"
#include "CameraComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotlightComponent.h"

#pragma comment(lib,"D3D12.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment(lib, "dxgi.lib")

#define D3D11_DOUBLE_SIDED D3D11_CULL_NONE

static constexpr UINT MAX_LIGHTS_PER_TYPE = 16u;

static constexpr UINT NUMBER_OF_BUFFERS = 2u;

using Microsoft::WRL::ComPtr;

//constexpr D3D11_INPUT_ELEMENT_DESC input_element_description[] = {
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//};

class GraphicsScene {
public:
	GraphicsScene() {}
	GraphicsScene(HWND window,
		const ObjectVector &objects);

	void tick();

	/*GET D3D11_PRIMITIVE_TOPOLOGY get_primitive_topology() const noexcept;
	void set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology) noexcept;

	GET Color get_background_color() const noexcept;
	void set_background_color(Color background_color) noexcept;

	GET D3D11_VIEWPORT get_viewport() const noexcept;
	void set_viewport(D3D11_VIEWPORT viewport) noexcept;*/

	GET std::shared_ptr<CameraComponent> camera() const;

	GET std::vector<std::shared_ptr<DirectionalLightComponent>> directional_lights() const;
	GET std::vector<std::shared_ptr<PointLightComponent>> point_lights() const;
	GET std::vector<std::shared_ptr<SpotlightComponent>> spotlights() const;

private:
	friend class Window;
	friend class Texture;

	GET std::shared_ptr<MeshComponent> obj_mesh(const Object &object) const;

	void create_essentials();

	bool vsync_enabled = false;
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	std::string video_card_description;
	ComPtr<IDXGISwapChain3> swap_chain = nullptr;
	ComPtr<ID3D12DescriptorHeap> render_target_view_heap = nullptr;
	ComPtr<ID3D12Resource> back_buffer_render_target[2] = { nullptr, nullptr };
	unsigned int buffer_index = 0u;
	ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;
	ComPtr<ID3D12PipelineState> pipeline_state = nullptr;
	ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fence_event = nullptr;
	ULONGLONG fence_value = 0ll;
	ComPtr<IDXGIFactory4> factory = nullptr;
	ComPtr<IDXGIAdapter> adapter = nullptr;
	ComPtr<IDXGIOutput> adapter_output = nullptr;

	//D3D11_VIEWPORT viewport = { };

	HWND window;

	ObjectVector objects;

	UVector2 resolution = UVector2(3840, 2160);

	FColor background_color = { 0.25f, 0.25f, 0.25f };
};
