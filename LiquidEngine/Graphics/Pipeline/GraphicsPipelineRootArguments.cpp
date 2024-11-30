#include "GraphicsPipelineRootArguments.h"

RootArgument::RootArgument(UINT parameter_index)
	: root_parameters{std::vector<D3D12_ROOT_PARAMETER>{1u}}, parameter_index{parameter_index} { }

UINT RootArgument::get_parameter_index() const noexcept {
	return parameter_index;
}

const std::vector<D3D12_ROOT_PARAMETER>& RootArgument::get_root_parameters() const noexcept {
	return root_parameters;
}

DescriptorTable::DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index, UINT parameter_index)
	: RootArgument{parameter_index}, ranges{std::vector<CD3DX12_DESCRIPTOR_RANGE>{1u}} {
	compile(type, shader, index);
}

void DescriptorTable::compile(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index) {
	ranges[0].Init(type, 1, index);

	table.NumDescriptorRanges = (UINT)ranges.size();
	table.pDescriptorRanges = &ranges[0];

	root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
	root_parameters[0].DescriptorTable = table; // this is our descriptor table for this root parameter
	root_parameters[0].ShaderVisibility = shader;
}

void DescriptorTable::clean_up() {
	root_parameters.clear();
	ranges.clear();
}

void DescriptorTable::set_descriptor_table(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap) {
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptor_heap.get()->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	command_list->SetGraphicsRootDescriptorTable(parameter_index, handle);
}

bool DescriptorTable::operator==(const DescriptorTable &descriptor_table) const noexcept {
	return (table == descriptor_table.table &&
		ranges[0] == descriptor_table.ranges[0]);
}

const D3D12_ROOT_DESCRIPTOR_TABLE & DescriptorTable::get_table() const noexcept {
	return table;
}

const std::vector<CD3DX12_DESCRIPTOR_RANGE> & DescriptorTable::get_ranges() const noexcept {
	return ranges;
}

void RootConstants::compile(D3D12_SHADER_VISIBILITY shader, UINT parameter_index, UINT index, UINT number_of_values) {
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
	root_parameters.clear();
}

void RootConstants::set_constants(const ComPtr<ID3D12GraphicsCommandList>& command_list) {
	command_list->SetGraphicsRoot32BitConstants(parameter_index, constants.Num32BitValues, obj, 0u);
}

bool RootConstants::operator==(const RootConstants& rc) const noexcept {
	return obj == rc.obj;
}

const D3D12_ROOT_CONSTANTS & RootConstants::get_constants() const noexcept {
	return constants;
}

void DescriptorRootObject::clean_up() {
	default_heap.Reset();
	heap_index = (UINT)-1;
	descriptor_table = nullptr;
}

void ConstantBuffer::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	if (heap_index == (UINT)-1) {
		heap_index = descriptor_heaps.get_open_heap_index();
		descriptor_table->heap_index = heap_index;
		descriptor_heaps.reserve_descriptor_index(heap_index);
	}

	if (valid()) {
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

		create_views(device, descriptor_heaps);

		update(device, command_list);
	}

	compile_signal = false;
}

void ConstantBuffer::create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		descriptor_heaps.create_cbv(device, default_heap, obj_size, i, heap_index);
	}
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
	UINT* gpu_address[1] = {};
	HPEW(upload_heap->Map(0u, &read_range, reinterpret_cast<void**>(&gpu_address[0])));
	memcpy(*gpu_address, obj, obj_size);

	command_list->CopyResource(default_heap.Get(), upload_heap.Get());

	update_signal = false;
}

bool ConstantBuffer::operator==(const ConstantBuffer &cb) const noexcept {
	return (obj == cb.obj &&
		obj_size == cb.obj_size);
}

ShaderResourceView::ShaderResourceView(const DirectX::ScratchImage &mip_chain, bool is_texture_cube)
	: DescriptorRootObject{} {
	update_descs(mip_chain, is_texture_cube);
}

void ShaderResourceView::update_descs(const DirectX::ScratchImage &mip_chain, bool is_texture_cube) {
	const Image &chain_base = *mip_chain.GetImages();
	heap_desc = D3D12_RESOURCE_DESC{
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Width = (UINT)chain_base.width,
		.Height = (UINT)chain_base.height,
		.DepthOrArraySize = 1,
		.MipLevels = (UINT16)mip_chain.GetImageCount(),
		.Format = chain_base.format,
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
			}) |
		std::ranges::to<std::vector>();

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
}

void ShaderResourceView::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	if (heap_index == (UINT)-1) {
		heap_index = descriptor_heaps.get_open_heap_index();
		descriptor_table->heap_index = heap_index;
		descriptor_heaps.reserve_descriptor_index(heap_index);
	}

	if (valid()) {
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

		create_views(device, descriptor_heaps);

		// write command to transition texture to texture state  
		/*{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			default_heap.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			command_list->ResourceBarrier(1, &barrier);
		}*/
	}

	compile_signal = false;
}

void ShaderResourceView::create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		descriptor_heaps.create_srv(device, default_heap, srv_desc, i, heap_index);
	}
}

bool ShaderResourceView::operator==(const ShaderResourceView& srv) const noexcept {
	return (heap_desc == srv.heap_desc);
}
