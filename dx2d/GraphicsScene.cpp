#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(HWND window, const ObjectVector &objects)
	: window(window), objects(objects) {
	Size2 size = get_window_size(window);

	create_essentials();
	create_unessentials();

	create_depth_stencil_buffer(size.x, size.y);

	D3D11_SAMPLER_DESC sampler_description = { };
	sampler_description.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_description.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_description.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_description.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_description.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_description.MinLOD = 0;
	sampler_description.MaxLOD = D3D11_FLOAT32_MAX;

	HANDLE_POSSIBLE_EXCEPTION(device->CreateSamplerState(&sampler_description, sampler_state.GetAddressOf()));

	context->IASetPrimitiveTopology(primitive_topology);

	context->OMSetRenderTargets(1u, target.GetAddressOf(), depth_stencil_view.Get());
}

// Anything related to object compilation for the graphics scene is done here.
void GraphicsScene::compile() {
	for (std::shared_ptr<Object> &object : *objects) {
		std::shared_ptr<MeshComponent> mesh = obj_mesh(*object);
		if (mesh != nullptr) {
			Material &material = mesh->material;

			if (material.texture != Texture()) {
				// +---------------- Texture ----------------+
				HANDLE_POSSIBLE_EXCEPTION(DirectX::CreateWICTextureFromFile(
					device.Get(),
					string_to_wstring(material.texture.file_name).c_str(),
					material.texture.texture.GetAddressOf(),
					material.texture.texture_view.GetAddressOf()
				));
			}
			if (material.normal_map != Texture()) {
				HANDLE_POSSIBLE_EXCEPTION(DirectX::CreateWICTextureFromFile(
					device.Get(),
					string_to_wstring(material.normal_map.file_name).c_str(),
					material.normal_map.texture.GetAddressOf(),
					material.normal_map.texture_view.GetAddressOf()
				));
			}

			// +---------------- Shaders ----------------+
			// Create Pixel Shader
			HANDLE_POSSIBLE_EXCEPTION(device->CreatePixelShader(material.pixel_blob->GetBufferPointer(), material.pixel_blob->GetBufferSize(), NULL, &material.pixel_shader));

			// Create Vertex Shader
			HANDLE_POSSIBLE_EXCEPTION(device->CreateVertexShader(material.vertex_blob->GetBufferPointer(), material.vertex_blob->GetBufferSize(), NULL, &material.vertex_shader));

			// Create Input Layout
			HANDLE_POSSIBLE_EXCEPTION(device->CreateInputLayout(input_element_description,
				(UINT)std::size(input_element_description),
				material.vertex_blob->GetBufferPointer(), 
				material.vertex_blob->GetBufferSize(),
				&material.input_layout
			));
		}
	}
}

void GraphicsScene::clean_up(bool clean_swap_chain, bool clean_device, bool clean_context) {
	for (std::shared_ptr<Object> &object : *objects) object->clean_up();

	COMPTR_RELEASE(depth_stencil_view);
	COMPTR_RELEASE(blend_state);
	COMPTR_RELEASE(rasterizer_state);
	COMPTR_RELEASE(target);
	if (clean_swap_chain)
		COMPTR_RELEASE(swap_chain);
	if (clean_context)
		COMPTR_RELEASE(context);
	if (clean_device)
		COMPTR_RELEASE(device);
}

void GraphicsScene::draw() {
	create_per_frame_constant_buffer();

	for (std::shared_ptr<Object> object : *objects) {
		std::shared_ptr<MeshComponent> mesh = obj_mesh(*object);
		if (mesh != nullptr) {
			set_vertex_buffer(*mesh);
			if (!mesh->mesh_data.indices.empty())
				set_index_buffer(*mesh);

			context->VSSetShader(mesh->material.vertex_shader.Get(), nullptr, 0u);
			context->PSSetShader(mesh->material.pixel_shader.Get(), nullptr, 0u);

			std::shared_ptr<CameraComponent> cam = camera();
			if (cam != nullptr) {
				create_per_object_constant_buffers(*cam, *object);
			}

			// Bind Input Layout
			context->IASetInputLayout(mesh->material.input_layout.Get());
			
			context->PSSetShaderResources(0, 1, mesh->material.texture.texture_view.GetAddressOf());
			context->PSSetShaderResources(1, 1, mesh->material.normal_map.texture_view.GetAddressOf());
			context->PSSetSamplers(0, 1, /*mesh->material.texture.*/sampler_state.GetAddressOf());

			// Draw it
			if (!mesh->mesh_data.indices.empty())
				context->DrawIndexed((UINT)mesh->mesh_data.indices.size(), 0u, 0u);
			else
				context->Draw((UINT)mesh->mesh_data.vertices.size(), 0u);
		}
	}
}

void GraphicsScene::present() {
	HANDLE_POSSIBLE_EXCEPTION(swap_chain->Present(1u, 0u));
}

void GraphicsScene::clear(bool clear_drawing) {
	const float color[] = { background_color.r, background_color.g, background_color.b, 1.0f };
	if (clear_drawing) context->ClearRenderTargetView(target.Get(), color);
	context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

D3D11_PRIMITIVE_TOPOLOGY GraphicsScene::get_primitive_topology() const noexcept {
	return primitive_topology;
}

void GraphicsScene::set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology) noexcept {
	this->primitive_topology = primitive_topology;
	context->IASetPrimitiveTopology(primitive_topology);
}

Color GraphicsScene::get_background_color() const noexcept {
	return background_color * 255;
}

void GraphicsScene::set_background_color(Color background_color) noexcept {
	this->background_color = color_to_fcolor(background_color) / 255;
}

D3D11_VIEWPORT GraphicsScene::get_viewport() const noexcept {
	return viewport;
}

void GraphicsScene::set_viewport(D3D11_VIEWPORT viewport) noexcept {
	this->viewport = viewport;
	context->RSSetViewports(1u, &viewport);
}

void GraphicsScene::create_depth_stencil_buffer(UINT width, UINT height) {
	// Create depth stensil state
	D3D11_DEPTH_STENCIL_DESC depth_stencil_description = {};
	depth_stencil_description.DepthEnable = true;
	depth_stencil_description.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_description.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	device->CreateDepthStencilState(&depth_stencil_description, &depth_stencil_state);

	// Bind depth state
	context->OMSetDepthStencilState(depth_stencil_state.Get(), 1u);

	//Create depth stensil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil;
	D3D11_TEXTURE2D_DESC desc_depth = { };
	desc_depth.Width = width;
	desc_depth.Height = height;
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
}

void GraphicsScene::set_vertex_buffer(const MeshComponent &mesh) {
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer = nullptr;

	// Buffer Description (Vertex)
	D3D11_BUFFER_DESC buffer_description = {  };
	buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_description.Usage = D3D11_USAGE_DEFAULT;
	buffer_description.CPUAccessFlags = 0u;
	buffer_description.MiscFlags = 0u;
	buffer_description.ByteWidth = sizeof(Vertex) * (UINT)mesh.mesh_data.vertices.size() + sizeof(Vertex);
	buffer_description.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA data = {  };
	data.pSysMem = &mesh.mesh_data.vertices[0];

	// Creating the Buffer
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&buffer_description, &data, &vertex_buffer));

	// Bind Vertex Buffer to Pipeline
	UINT stride = sizeof(Vertex);
	UINT offset = 0u;
	context->IASetVertexBuffers(0u, 1u, vertex_buffer.GetAddressOf(), &stride, &offset);
}

void GraphicsScene::set_index_buffer(const MeshComponent &mesh) {
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer = nullptr;
	D3D11_BUFFER_DESC index_buffer_description = { };
	index_buffer_description.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_description.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_description.CPUAccessFlags = 0u;
	index_buffer_description.MiscFlags = 0u;
	index_buffer_description.ByteWidth = sizeof(USHORT) * (UINT)mesh.mesh_data.indices.size() + sizeof(USHORT);
	index_buffer_description.StructureByteStride = sizeof(USHORT);
	D3D11_SUBRESOURCE_DATA subresource_data = { };
	subresource_data.pSysMem = &mesh.mesh_data.indices[0];
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&index_buffer_description, &subresource_data, &index_buffer));
	// Bind index buffer
	context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

void GraphicsScene::create_per_object_constant_buffers(CameraComponent &camera, const Object &object) {
	PerObjectVertexConstantBuffer vcb;

	Transform object_transform = object.get_transform();

	Size2 size = get_window_size(window);
	camera.update(size);

	vcb.WVP = XMMatrixTranspose(camera.WVP);
	vcb.transform = object_transform;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_constant_buffer_pointer;
	D3D11_BUFFER_DESC vertex_constant_buffer_description;
	vertex_constant_buffer_description.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	vertex_constant_buffer_description.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertex_constant_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	vertex_constant_buffer_description.MiscFlags = 0u;
	vertex_constant_buffer_description.ByteWidth = sizeof(vcb);
	vertex_constant_buffer_description.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA vertex_constant_subresource_data = { };
	vertex_constant_subresource_data.pSysMem = &vcb;
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&vertex_constant_buffer_description, &vertex_constant_subresource_data, &vertex_constant_buffer_pointer));

	// Set constant buffer
	context->VSSetConstantBuffers(1u, 1u, vertex_constant_buffer_pointer.GetAddressOf());

	PerObjectPixelConstantBuffer pcb;
	pcb.material = (Material::ConstantBufferStruct)obj_mesh(object)->material;

	Microsoft::WRL::ComPtr<ID3D11Buffer> material_constant_buffer_pointer;
	D3D11_BUFFER_DESC material_constant_buffer_description;
	material_constant_buffer_description.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	material_constant_buffer_description.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	material_constant_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	material_constant_buffer_description.MiscFlags = 0u;
	material_constant_buffer_description.ByteWidth = sizeof(pcb);
	material_constant_buffer_description.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA material_constant_subresource_data = { };
	material_constant_subresource_data.pSysMem = &pcb;
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&material_constant_buffer_description, &material_constant_subresource_data, &material_constant_buffer_pointer));

	// Set constant buffer
	context->PSSetConstantBuffers(2u, 1u, material_constant_buffer_pointer.GetAddressOf());
}

void GraphicsScene::create_per_frame_constant_buffer() {
	PerFrameConstantBuffer cb;

	std::vector<std::shared_ptr<DirectionalLightComponent>> dls = directional_lights();
	std::vector<DirectionalLightComponent::ConstantBufferStruct> cbdls;
	std::for_each(dls.begin(), dls.end(),
		[&cbdls](std::shared_ptr<DirectionalLightComponent> dl) {
			cbdls.push_back((DirectionalLightComponent::ConstantBufferStruct)*dl);
		}
	);

	std::copy(cbdls.begin(), cbdls.end(), cb.directional_lights);
	cb.directional_light_count = (uint)cbdls.size();

	std::vector<std::shared_ptr<PointLightComponent>> pls = point_lights();
	std::vector<PointLightComponent::ConstantBufferStruct> cbpls;
	std::for_each(pls.begin(), pls.end(),
		[&cbpls](std::shared_ptr<PointLightComponent> pl) {
			cbpls.push_back((PointLightComponent::ConstantBufferStruct)*pl);
		}
	);

	std::copy(cbpls.begin(), cbpls.end(), cb.point_lights);
	cb.point_light_count = (uint)cbpls.size();

	std::vector<std::shared_ptr<SpotlightComponent>> sls = spotlights();
	std::vector<SpotlightComponent::ConstantBufferStruct> cbsls;
	std::for_each(sls.begin(), sls.end(),
		[&cbsls](std::shared_ptr<SpotlightComponent> sl) {
			cbsls.push_back((SpotlightComponent::ConstantBufferStruct)*sl);
		}
	);

	std::copy(cbsls.begin(), cbsls.end(), cb.spotlights);
	cb.spotlight_count = (uint)cbsls.size();

	cb.camera_position = camera()->get_position();

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_pointer;
	D3D11_BUFFER_DESC constant_buffer_description;
	constant_buffer_description.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_description.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	constant_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	constant_buffer_description.MiscFlags = 0u;
	constant_buffer_description.ByteWidth = sizeof(cb);
	constant_buffer_description.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA constant_subresource_data = { };
	constant_subresource_data.pSysMem = &cb;
	constant_subresource_data.SysMemPitch = 0u;
	constant_subresource_data.SysMemSlicePitch = 0u;
	HANDLE_POSSIBLE_EXCEPTION(device->CreateBuffer(&constant_buffer_description, &constant_subresource_data, &constant_buffer_pointer));

	// Set constant buffer
	context->PSSetConstantBuffers(0u, 1u, constant_buffer_pointer.GetAddressOf());
}

void GraphicsScene::create_essentials() {
	Size2 size = get_window_size(window);

	DXGI_SWAP_CHAIN_DESC swap_chain_description = { };
	swap_chain_description.BufferDesc.Width = size.x; // Figure it out yourself
	swap_chain_description.BufferDesc.Height = size.y;
	swap_chain_description.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // Or DXGI_FORMAT_B8G8R8A8_UNORM? // Layout of pixels
	swap_chain_description.BufferDesc.RefreshRate.Numerator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.RefreshRate.Denominator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // Don't need scaling
	swap_chain_description.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Scanline order (Interlaced, etc)
	swap_chain_description.SampleDesc.Count = 1; // No anti-aliasing
	swap_chain_description.SampleDesc.Quality = 0; // No anti-aliasing
	swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use buffer as render target
	swap_chain_description.BufferCount = 1; // One back buffer
	swap_chain_description.OutputWindow = window; // Use this window
	swap_chain_description.Windowed = true; // Is it windowed?
	swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; // Swap effect that usually has the best performance
	swap_chain_description.Flags = 0u; // No flags

	HANDLE_POSSIBLE_EXCEPTION(D3D11CreateDeviceAndSwapChain(
		nullptr, // Choose default adapter
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, // Use hardware device
		nullptr, // Handle to driver
		D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG,
		nullptr, // Give us any feature level
		0,
		D3D11_SDK_VERSION, // Default SDK Version
		&swap_chain_description, // Swap chain description
		&swap_chain, // Swap Chain
		&device, // Device
		nullptr, // Output pointer - Sets to feature level
		&context // Context
	));

	DXGI_MODE_DESC bufferDesc = { };
	bufferDesc.Width = size.x;
	bufferDesc.Height = size.y;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer = nullptr;
	HANDLE_POSSIBLE_EXCEPTION(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer));
	HANDLE_POSSIBLE_EXCEPTION(device->CreateRenderTargetView(
		back_buffer.Get(),
		nullptr,
		&target
	));

	// Configure Viewport
	viewport.Width = (float)size.x;
	viewport.Height = (float)size.y;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	context->RSSetViewports(1u, &viewport);
}

void GraphicsScene::create_unessentials() {
	Size2 size = get_window_size(window);

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

	/*D3D11_RECT rect;
	rect.left = 0;
	rect.right = width;
	rect.top = 0;
	rect.bottom = height;

	context->RSSetScissorRects(1, &rect);*/

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
}

std::shared_ptr<CameraComponent> GraphicsScene::camera() const {
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::CameraComponent)) {
			return object->get_component<CameraComponent>();
		}
	}
	WARNING_MESSAGE(L"There is no camera in the scene.");
	return nullptr;
}

std::vector<std::shared_ptr<DirectionalLightComponent>> GraphicsScene::directional_lights() const {
	std::vector<std::shared_ptr<DirectionalLightComponent>> ret;
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::DirectionalLightComponent)) {
			std::vector<std::shared_ptr<DirectionalLightComponent>> dls = object->get_components<DirectionalLightComponent>();
			ret.insert(ret.end(), dls.begin(), dls.end());
		}
	}
	return ret;
}

std::vector<std::shared_ptr<PointLightComponent>> GraphicsScene::point_lights() const {
	std::vector<std::shared_ptr<PointLightComponent>> ret;
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::PointLightComponent)) {
			std::vector<std::shared_ptr<PointLightComponent>> dls = object->get_components<PointLightComponent>();
			ret.insert(ret.end(), dls.begin(), dls.end());
		}
	}
	return ret;
}

std::vector<std::shared_ptr<SpotlightComponent>> GraphicsScene::spotlights() const {
	std::vector<std::shared_ptr<SpotlightComponent>> ret;
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::SpotlightComponent)) {
			std::vector<std::shared_ptr<SpotlightComponent>> dls = object->get_components<SpotlightComponent>();
			ret.insert(ret.end(), dls.begin(), dls.end());
		}
	}
	return ret;
}

std::shared_ptr<MeshComponent> GraphicsScene::obj_mesh(const Object &object) const {
	return object.get_component<MeshComponent>();
}
