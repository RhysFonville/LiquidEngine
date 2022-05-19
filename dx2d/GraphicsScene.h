#pragma once

#include <d3d11.h>
#include <exception>
#include <wrl.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Throw.h"
#include "Object.h"

#define D3D11_DOUBLE_SIDED D3D11_CULL_NONE

#pragma comment(lib,"D3D11.lib")
#pragma comment (lib, "D3DCompiler.lib")

class GraphicsScene {
public:
	GraphicsScene() {}
	GraphicsScene(HWND window,
		const std::shared_ptr<std::vector<Object>> &objects);

	void draw();
	void present();
	void clear(bool clear_drawing = true);

	D3D11_PRIMITIVE_TOPOLOGY get_primitive_topology();
	void set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology);

	Color get_background_color();
	void set_background_color(Color background_color);

	D3D11_VIEWPORT get_viewport();
	void set_viewport(D3D11_VIEWPORT viewport);

	D3D11_FILL_MODE fill_mode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	D3D11_CULL_MODE cull_mode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

private:
	friend class Window;

	void set_vertex_buffer(Object object);
	void set_index_buffer(Object object);

	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain = nullptr;
	DXGI_SWAP_CHAIN_DESC swap_chain_description = { };
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> target = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view = nullptr;

	D3D11_VIEWPORT viewport = { };

	HWND window;

	std::shared_ptr<std::vector<Object>> objects;

	Color background_color = { 0, 0, 0 };
	D3D11_PRIMITIVE_TOPOLOGY primitive_topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
