#pragma once

#include <DirectXMath.h>
#include <filesystem>
#include "Object.h"
#include "globalutil.h"
#include "CameraComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotlightComponent.h"

#define D3D11_DOUBLE_SIDED D3D11_CULL_NONE

#pragma comment(lib,"D3D11.lib")
#pragma comment (lib, "D3DCompiler.lib")

#define MAX_LIGHTS_PER_TYPE 16

#pragma pack(4)

constexpr D3D11_INPUT_ELEMENT_DESC input_element_description[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

__declspec(align(16))
struct PerFrameConstantBuffer {
	XMFLOAT3 pad;
	DirectionalLightComponent::ConstantBufferStruct directional_lights[MAX_LIGHTS_PER_TYPE] = { };
	PointLightComponent::ConstantBufferStruct point_lights[MAX_LIGHTS_PER_TYPE] = { };
	SpotlightComponent::ConstantBufferStruct spotlights[MAX_LIGHTS_PER_TYPE] = { };
	uint directional_light_count = 0;
	uint point_light_count = 0;
	uint spotlight_count = 0;

	float pad1;

	XMFLOAT3 camera_position;
};

__declspec(align(16))
struct PerObjectVertexConstantBuffer {
	XMMATRIX WVP;
	XMMATRIX transform;
};

__declspec(align(16))
struct PerObjectPixelConstantBuffer {
	Material::ConstantBufferStruct material;
};

class GraphicsScene {
public:
	GraphicsScene() {}
	GraphicsScene(HWND window,
		const ObjectVector &objects);

	void compile();
	void clean_up(bool clean_swap_chain = true, bool clean_device = true, bool clean_context = true);

	void draw();
	void present();
	void clear(bool clear_drawing = true);

	GET D3D11_PRIMITIVE_TOPOLOGY get_primitive_topology() const noexcept;
	void set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology) noexcept;

	GET Color get_background_color() const noexcept;
	void set_background_color(Color background_color) noexcept;

	GET D3D11_VIEWPORT get_viewport() const noexcept;
	void set_viewport(D3D11_VIEWPORT viewport) noexcept;

	D3D11_FILL_MODE fill_mode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	D3D11_CULL_MODE cull_mode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	D3D11_PRIMITIVE_TOPOLOGY primitive_topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	GET std::shared_ptr<CameraComponent> camera() const;

	GET std::vector<std::shared_ptr<DirectionalLightComponent>> directional_lights() const;
	GET std::vector<std::shared_ptr<PointLightComponent>> point_lights() const;
	GET std::vector<std::shared_ptr<SpotlightComponent>> spotlights() const;

private:
	friend class Window;
	friend class Texture;

	GET std::shared_ptr<MeshComponent> obj_mesh(const Object &object) const;

	void create_depth_stencil_buffer(UINT width, UINT height);

	void set_vertex_buffer(const MeshComponent &mesh);
	void set_index_buffer(const MeshComponent &mesh);

	void create_per_object_constant_buffers(CameraComponent &camera, const Object &object);
	void create_per_frame_constant_buffer();

	void create_essentials();
	void create_unessentials();

	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> target = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view = nullptr;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state = nullptr;

	D3D11_VIEWPORT viewport = { };

	HWND window;

	ObjectVector objects;

	UVector2 resolution = UVector2(1920, 1080);

	FColor background_color = { 0.25f, 0.25f, 0.25f };
};
