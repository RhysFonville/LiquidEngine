#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(HWND window, const std::shared_ptr<std::vector<Object>> &objects)
	: window(window), objects(objects) {
	UINT creationFlags = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
	creationFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swap_chain_description = { };
	swap_chain_description.BufferDesc.Width = 0; // Figure it out yourself
	swap_chain_description.BufferDesc.Height = 0;
	swap_chain_description.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM; // Layout of pixels
	swap_chain_description.BufferDesc.RefreshRate.Numerator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.RefreshRate.Denominator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // Don't need scaling
	swap_chain_description.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Scanline order (Interlaced, etc)
	swap_chain_description.SampleDesc.Count = 1; // No anti-aliasing
	swap_chain_description.SampleDesc.Quality = 0; // No anti-aliasing
	swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use buffer as render target
	swap_chain_description.BufferCount = 1; // One back buffer
	swap_chain_description.OutputWindow = window; // Use this window
	swap_chain_description.Windowed = true; // We want it windowed
	swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; // Swap effect that usually has the best performance
	swap_chain_description.Flags = 0u; // No flags

	HANDLE_POSSIBLE_EXCEPTION(D3D11CreateDeviceAndSwapChain(
		nullptr, // Choose default adapter
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, // Use hardware device
		nullptr, // Handle to driver
		0, // No flags
		nullptr, // Give us any feature level
		0,
		D3D11_SDK_VERSION, // Default SDK Version
		&swap_chain_description, // Swap chain description
		&swap_chain, // Swap Chain
		&device, // Device
		nullptr, // Output pointer - Sets to feature level
		&context // Context
	));

	Microsoft::WRL::ComPtr<ID3D11Resource> back_buffer = nullptr;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Resource), &back_buffer);
	HANDLE_POSSIBLE_EXCEPTION(device->CreateRenderTargetView(
		back_buffer.Get(),
		nullptr,
		&target
	));

	// Create depth stensil state
	D3D11_DEPTH_STENCIL_DESC depth_stencil_description = {};
	depth_stencil_description.DepthEnable = true;
	depth_stencil_description.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_description.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	device->CreateDepthStencilState(&depth_stencil_description, &depth_stencil_state);

	// Bind depth state
	context->OMSetDepthStencilState(depth_stencil_state.Get(), 1u);

	//Create depth stensil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil;
	D3D11_TEXTURE2D_DESC desc_depth = { };
	desc_depth.Width = 800u;
	desc_depth.Height = 600u;
	desc_depth.MipLevels = 1u;
	desc_depth.ArraySize = 1u;
	desc_depth.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	desc_depth.SampleDesc.Count = 1u;
	desc_depth.SampleDesc.Quality = 0u;
	desc_depth.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	desc_depth.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	device->CreateTexture2D(&desc_depth, nullptr, &depth_stencil);

	// Create depth stensil state
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = { };
	descDSV.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(depth_stencil.Get(), &descDSV, &depth_stencil_view);

	/*D3D11_RECT rect;
	rect.left = 0;
	rect.right = 800;
	rect.top = 0;
	rect.bottom = 600;

	context->RSSetScissorRects(1, &rect);*/
	
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state = nullptr;
	D3D11_RASTERIZER_DESC rasterizer_state_description = { };
	rasterizer_state_description.FillMode = fill_mode;
	rasterizer_state_description.CullMode = cull_mode;
	rasterizer_state_description.FrontCounterClockwise = true;
	rasterizer_state_description.DepthBias = false;
	rasterizer_state_description.DepthBiasClamp = 0;
	rasterizer_state_description.SlopeScaledDepthBias = 0;
	rasterizer_state_description.DepthClipEnable = true;
	rasterizer_state_description.ScissorEnable = false;
	rasterizer_state_description.MultisampleEnable = false;
	rasterizer_state_description.AntialiasedLineEnable = false;
	HANDLE_POSSIBLE_EXCEPTION(device->CreateRasterizerState(&rasterizer_state_description, rasterizer_state.GetAddressOf()));

	context->RSSetState(rasterizer_state.Get());

	// Configure Viewport
	viewport.Width = 800u;
	viewport.Height = 600u;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	context->RSSetViewports(1u, &viewport);

	D3D11_BLEND_DESC blend_description = { };
	blend_description.RenderTarget[0].BlendEnable = true;
	blend_description.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	blend_description.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	blend_description.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	blend_description.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	blend_description.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	blend_description.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	blend_description.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBlendState(&blend_description, &blend_state));

	context->OMSetBlendState(blend_state.Get(), 0, 0xffffffff);

	// Set Primitive Topology to triangle list
	set_primitive_topology(primitive_topology);

	context->OMSetRenderTargets(1u, target.GetAddressOf(), depth_stencil_view.Get());
}

void GraphicsScene::compile() {
	for (Object &object : *objects) {
		Material &material = object.appearance.material;

		// +---------------- Texture ----------------+
		HANDLE_POSSIBLE_EXCEPTION(DirectX::CreateWICTextureFromFile(
			device.Get(),
			string_to_wstring(material.texture.file_name).c_str(),
			material.texture.texture.GetAddressOf(),
			material.texture.texture_view.GetAddressOf()
		));

		D3D11_SAMPLER_DESC sampler_description = { };
		sampler_description.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_description.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_description.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_description.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_description.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler_description.MinLOD = 0;
		sampler_description.MaxLOD = D3D11_FLOAT32_MAX;

		HANDLE_POSSIBLE_EXCEPTION(device->CreateSamplerState(&sampler_description, material.texture.sampler_state.GetAddressOf()));

		// +---------------- Shaders ----------------+
		// Create Pixel Shader
		HANDLE_POSSIBLE_EXCEPTION(device->CreatePixelShader(material.pixel_blob->GetBufferPointer(), material.pixel_blob->GetBufferSize(), nullptr, &material.pixel_shader));

		// Set Pixel Shader
		context->PSSetShader(material.pixel_shader.Get(), nullptr, 0u);

		// Create Vertex Shader
		HANDLE_POSSIBLE_EXCEPTION(device->CreateVertexShader(material.vertex_blob->GetBufferPointer(), material.vertex_blob->GetBufferSize(), nullptr, &material.vertex_shader));

		// Set Vertex Shader
		context->VSSetShader(material.vertex_shader.Get(), nullptr, 0u);

		// Input layout are the parameters for the shaders
		const D3D11_INPUT_ELEMENT_DESC input_element_description[] = {
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		};

		// Create Input Layout
		HANDLE_POSSIBLE_EXCEPTION(device->CreateInputLayout(input_element_description,
			(UINT)std::size(input_element_description),
			material.vertex_blob->GetBufferPointer(), 
			material.vertex_blob->GetBufferSize(),
			&material.input_layout
		));
	}
}

/*void GraphicsScene::clean_up() {
	device.Get()->Release();
	device->Release();

	swap_chain.Get()->Release();
	swap_chain->Release();

	context.Get()->Release();
	context->Release();

	target.Get()->Release();
	target->Release();

	depth_stencil_view.Get()->Release();
	depth_stencil_view->Release();

	blend_state.Get()->Release();
	blend_state->Release();

	for (Object &object : *objects) {
		object.appearance.material.input_layout.Get()->Release();
		object.appearance.material.input_layout->Release();

		object.appearance.material.pixel_blob.Get()->Release();
		object.appearance.material.pixel_blob->Release();

		object.appearance.material.pixel_shader.Get()->Release();
		object.appearance.material.pixel_shader->Release();

		object.appearance.material.texture.sampler_state.Get()->Release();
		object.appearance.material.texture.sampler_state->Release();

		object.appearance.material.texture.texture.Get()->Release();
		object.appearance.material.texture.texture->Release();

		object.appearance.material.texture.texture_view.Get()->Release();
		object.appearance.material.texture.texture_view->Release();

		object.appearance.material.vertex_blob.Get()->Release();
		object.appearance.material.vertex_blob->Release();

		object.appearance.material.vertex_shader.Get()->Release();
		object.appearance.material.vertex_shader->Release();
	}
}*/

void GraphicsScene::draw() {
	for (Object object : *objects) {
		set_vertex_buffer(object);
		set_index_buffer(object);

		const XMMATRIX transform_buffer = object.transform;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_pointer;
		D3D11_BUFFER_DESC constant_buffer_description;
		constant_buffer_description.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		constant_buffer_description.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		constant_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		constant_buffer_description.MiscFlags = 0u;
		constant_buffer_description.ByteWidth = sizeof(transform_buffer);
		constant_buffer_description.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA constant_subresource_data = { };
		constant_subresource_data.pSysMem = &transform_buffer;
		HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&constant_buffer_description, &constant_subresource_data, &constant_buffer_pointer));

		// Set constant buffer
		context->VSSetConstantBuffers(0u, 1u, constant_buffer_pointer.GetAddressOf());

		// Bind Input Layout
		context->IASetInputLayout(object.appearance.material.input_layout.Get());

		context->PSSetShaderResources(0, 1, object.appearance.material.texture.texture_view.GetAddressOf());
		context->PSSetSamplers(0, 1, object.appearance.material.texture.sampler_state.GetAddressOf());

		// Draw it
		context->DrawIndexed((UINT)object.appearance.mesh.indices.size(), 0u, 0u);
	}
}


void GraphicsScene::present() {
	HANDLE_POSSIBLE_EXCEPTION(swap_chain->Present(1u, 0u));
}

void GraphicsScene::clear(bool clear_drawing) {
	const float color[] = { (float)background_color.r, (float)background_color.g, (float)background_color.b, 1.0f };
	if (clear_drawing) context->ClearRenderTargetView(target.Get(), color);
	context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

D3D11_PRIMITIVE_TOPOLOGY GraphicsScene::get_primitive_topology() {
	return primitive_topology;
}

void GraphicsScene::set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology) {
	this->primitive_topology = primitive_topology;
	context->IASetPrimitiveTopology(primitive_topology);
}

Color GraphicsScene::get_background_color() {
	return background_color;
}

void GraphicsScene::set_background_color(Color primitive_topology) {
	this->background_color = background_color;
}

void GraphicsScene::set_vertex_buffer(Object object) {
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer = nullptr;

	// Buffer Description (Vertex)
	D3D11_BUFFER_DESC buffer_description = {  };
	buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_description.Usage = D3D11_USAGE_DEFAULT;
	buffer_description.CPUAccessFlags = 0u;
	buffer_description.MiscFlags = 0u;
	buffer_description.ByteWidth = sizeof(Vertex) * (UINT)object.appearance.mesh.vertices.size() + sizeof(Vertex);
	buffer_description.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA data = {  };
	data.pSysMem = &object.appearance.mesh.vertices[0];

	// Creating the Buffer
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&buffer_description, &data, &vertex_buffer));

	// Bind Vertex Buffer to Pipeline
	UINT stride = sizeof(Vertex);
	UINT offset = 0u;
	context->IASetVertexBuffers(0u, 1u, vertex_buffer.GetAddressOf(), &stride, &offset);
}

void GraphicsScene::set_index_buffer(Object object) {
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer = nullptr;
	D3D11_BUFFER_DESC index_buffer_description = { };
	index_buffer_description.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_description.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_description.CPUAccessFlags = 0u;
	index_buffer_description.MiscFlags = 0u;
	index_buffer_description.ByteWidth = sizeof(USHORT) * (UINT)object.appearance.mesh.indices.size() + sizeof(USHORT);
	index_buffer_description.StructureByteStride = sizeof(USHORT);
	D3D11_SUBRESOURCE_DATA subresource_data = { };
	subresource_data.pSysMem = &object.appearance.mesh.indices[0];
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&index_buffer_description, &subresource_data, &index_buffer));
	// Bind index buffer
	context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

D3D11_VIEWPORT GraphicsScene::get_viewport() {
	return viewport;
}

void GraphicsScene::set_viewport(D3D11_VIEWPORT viewport) {
	this->viewport = viewport;
	context->RSSetViewports(1u, &viewport);
}