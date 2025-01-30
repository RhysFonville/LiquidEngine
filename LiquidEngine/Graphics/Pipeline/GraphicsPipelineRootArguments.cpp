#include "GraphicsPipelineRootArguments.h"

RootArgument::RootArgument(UINT index) : index{index} { }

UINT RootArgument::get_index() const noexcept {
	return index;
}

DescriptorTable::DescriptorTable(const D3D12_DESCRIPTOR_RANGE1& range, UINT param_index)
	: range{range}, RootArgument{param_index} { }

DescriptorTable::DescriptorTable(D3D12_SHADER_INPUT_BIND_DESC bind_desc, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT param_index)
	: DescriptorTable{D3D12_DESCRIPTOR_RANGE1{
		type,
		1u,
		bind_desc.BindPoint,
		bind_desc.Space,
		D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC
	}, param_index} { }

DescriptorTable::DescriptorTable(const DescriptorTable& dt)
	: DescriptorTable{dt.range, dt.index} { }

D3D12_ROOT_PARAMETER1 DescriptorTable::get_root_param() const noexcept {
	return D3D12_ROOT_PARAMETER1{
		.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
		.DescriptorTable{
			.NumDescriptorRanges = 1u,
			.pDescriptorRanges = &range
		}
	};
}

void DescriptorTable::set_descriptor_table(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap) const {
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptor_heap.get()->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	command_list->SetGraphicsRootDescriptorTable(index, handle);
}

bool DescriptorTable::operator==(const DescriptorTable &descriptor_table) const noexcept {
	return (range == descriptor_table.range);
}

D3D12_ROOT_DESCRIPTOR_TABLE1 DescriptorTable::get_table() const noexcept {
	return get_root_param().DescriptorTable;
}

const D3D12_DESCRIPTOR_RANGE1& DescriptorTable::get_range() const noexcept {
	return range;
}

/*void RootConstants::compile(D3D12_SHADER_VISIBILITY shader, UINT parameter_index, UINT index, UINT number_of_values) {
	this->parameter_index = parameter_index;
	this->root_parameters.resize(1u);

	constants.Num32BitValues = (number_of_values == (UINT)-1 ? (UINT)std::ceilf((float)obj_size * 8.0f / 32.0f) : number_of_values);
	constants.RegisterSpace = 0u;
	constants.ShaderRegister = index;

	root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	root_parameters[0].Constants = constants;
	root_parameters[0].ShaderVisibility = shader;
}

void RootConstants::clean_up() {
	obj = nullptr;
}

void RootConstants::set_constants(const ComPtr<ID3D12GraphicsCommandList>& command_list) {
	command_list->SetGraphicsRoot32BitConstants(parameter_index, constants.Num32BitValues, obj, 0u);
}

bool RootConstants::operator==(const RootConstants& rc) const noexcept {
	return obj == rc.obj;
}

const D3D12_ROOT_CONSTANTS & RootConstants::get_constants() const noexcept {
	return constants;
}*/

bool DescriptorRootObject::operator==(const DescriptorRootObject& dro) const noexcept {
	return (descriptor_table == dro.descriptor_table);
}

void DescriptorRootObject::compile() noexcept {
	compile_signal = true;
}

bool DescriptorRootObject::will_compile() const noexcept {
	return compile_signal;
}

void DescriptorRootObject::clean_up() {
	default_heap.Reset();
}

RootConstants::RootConstants(D3D12_SHADER_INPUT_BIND_DESC bind_desc, D3D12_SHADER_BUFFER_DESC buffer_desc, UINT param_index)
	: RootArgument{param_index},
	constants{
		.ShaderRegister = bind_desc.BindPoint,
		.RegisterSpace = bind_desc.Space,
		.Num32BitValues = (UINT)ceil(buffer_desc.Size/4u)
	} { }

bool RootConstants::operator==(const RootConstants& rc) const noexcept {
	return (obj == rc.obj && obj_size == rc.obj_size);
}

D3D12_ROOT_PARAMETER1 RootConstants::get_root_param() const noexcept {
	return D3D12_ROOT_PARAMETER1{
		.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
		.Constants = constants
	};
}

GET const D3D12_ROOT_CONSTANTS& RootConstants::get_constants() const noexcept {
	return constants;
}


void RootConstants::set_constants(const ComPtr<ID3D12GraphicsCommandList>& command_list) {
	command_list->SetGraphicsRoot32BitConstants(index, constants.Num32BitValues, obj, 0u);
}

void ConstantBuffer::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	descriptor_table.heap_index = descriptor_heaps.get_open_heap_index();
	descriptor_heaps.reserve_descriptor_index(descriptor_table.heap_index);

	auto props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto buf = CD3DX12_RESOURCE_DESC::Buffer((obj_size + 255) & ~255);
	HPEW(device->CreateCommittedResource(
		&props,
		D3D12_HEAP_FLAG_NONE,
		&buf,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&default_heap)
	));
	HPEW(default_heap->SetName(L"CB default heap"));

	create_view(device, descriptor_heaps);

	update(device, command_list);

	compile_signal = false;
}

void ConstantBuffer::create_view(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	descriptor_heaps.create_cbv(device, default_heap, obj_size, descriptor_table.heap_index);
}

void ConstantBuffer::update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list) {
	ComPtr<ID3D12Resource> upload_heap{nullptr};
	auto type = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto buf = CD3DX12_RESOURCE_DESC::Buffer((obj_size + 255) & ~255);
	HPEW(device->CreateCommittedResource(
		&type, // this heap will be used to upload the constant buffer data
		D3D12_HEAP_FLAG_NONE, // no flags
		&buf, // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
		D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
		nullptr, // we do not have use an optimized clear value for constant buffers
		IID_PPV_ARGS(&upload_heap)));

	resource_manager->add_resource_to_release(upload_heap);
	HPEW(upload_heap->SetName(L"CB upload heap"));

	CD3DX12_RANGE read_range{0, 0};	// We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
	UINT* gpu_address[1]{};
	HPEW(upload_heap->Map(0u, &read_range, reinterpret_cast<void**>(&gpu_address[0])));
	memcpy(*gpu_address, obj, obj_size);

	command_list->CopyResource(default_heap.Get(), upload_heap.Get());

	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			default_heap.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		command_list->ResourceBarrier(1, &barrier);
	}

	update_signal = false;
}

void ConstantBuffer::clean_up() {
	obj = nullptr;
}

bool ConstantBuffer::operator==(const ConstantBuffer &cb) const noexcept {
	return (obj == cb.obj &&
		obj_size == cb.obj_size);
}

void ShaderResourceView::update_descs(const DirectX::ScratchImage &mip_chain, bool is_texture_cube) {
	const DirectX::Image* chain_base = mip_chain.GetImages();
	if (chain_base == nullptr) return;

	heap_desc = D3D12_RESOURCE_DESC{
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Width = (UINT)chain_base->width,
		.Height = (UINT)chain_base->height,
		.DepthOrArraySize = 1,
		.MipLevels = (UINT16)mip_chain.GetImageCount(),
		.Format = chain_base->format,
		.SampleDesc = {.Count = 1},
		.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		.Flags = D3D12_RESOURCE_FLAG_NONE,
	};

	// collect subresource data 
	subresources = std::ranges::views::iota(0, (int)mip_chain.GetImageCount()) |
		std::ranges::views::transform([&](int i) {
		const auto img = mip_chain.GetImage(i, 0, 0);
		return D3D12_SUBRESOURCE_DATA{
			.pData = img->pixels,
			.RowPitch = (LONG_PTR)img->rowPitch,
			.SlicePitch = (LONG_PTR)img->slicePitch,
		};
	}) | std::ranges::to<std::vector>();

	srv_desc = D3D12_SHADER_RESOURCE_VIEW_DESC{
		.Format = heap_desc.Format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = heap_desc.MipLevels},
	};

	if (is_texture_cube) {
		srv_desc.TextureCube.MipLevels = (UINT)mip_chain.GetImageCount();
	} else {
		srv_desc.Texture2D.MipLevels = (UINT)mip_chain.GetImageCount();
	}

	compile();
}

void ShaderResourceView::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	descriptor_table.heap_index = descriptor_heaps.get_open_heap_index();
	descriptor_heaps.reserve_descriptor_index(descriptor_table.heap_index);

	if (default_heap != nullptr) default_heap.Reset();

	auto props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	HPEW(device->CreateCommittedResource(
		&props,
		D3D12_HEAP_FLAG_NONE,
		&heap_desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&default_heap)
	));
	HPEW(default_heap->SetName(string_to_wstring("SRV Default heap").c_str()));

	// create the intermediate upload buffer 
	ComPtr<ID3D12Resource> upload_heap{nullptr};
	const CD3DX12_HEAP_PROPERTIES heapProps{D3D12_HEAP_TYPE_UPLOAD};
	const auto upload_heap_size = GetRequiredIntermediateSize(
		default_heap.Get(), 0, (UINT)subresources.size()
	);
	const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(upload_heap_size);
	HPEW(device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&upload_heap)
	));

	resource_manager->add_resource_to_release(upload_heap);
	HPEW(upload_heap->SetName(L"SRV upload buffer"));

	// write commands to copy data to upload texture (copying each subresource) 
	UpdateSubresources(
		command_list.Get(),
		default_heap.Get(),
		upload_heap.Get(),
		0, 0,
		(UINT)subresources.size(),
		subresources.data()
	);

	create_view(device, descriptor_heaps);

	// write command to transition texture to texture state  
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			default_heap.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		);
		command_list->ResourceBarrier(1, &barrier);
	}

	compile_signal = false;
}

void ShaderResourceView::clean_up() {
	subresources.clear();
}

void ShaderResourceView::create_view(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	descriptor_heaps.create_srv(device, default_heap, srv_desc, descriptor_table.heap_index);
}

bool ShaderResourceView::operator==(const ShaderResourceView& srv) const noexcept {
	return (heap_desc == srv.heap_desc);
}
