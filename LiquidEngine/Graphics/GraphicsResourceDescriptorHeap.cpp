#include "GraphicsResourceDescriptorHeap.h"

GraphicsResourceDescriptorHeap::GraphicsResourceDescriptorHeap() { }

void GraphicsResourceDescriptorHeap::compile(const ComPtr<ID3D12Device> &device) {
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
	heap_desc.NumDescriptors = 100000; // 100,000 descriptors -> 0.4 megabytes
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heap_desc.NodeMask = 0u;

	HPEW(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&descriptor_heap)));
	HPEW(descriptor_heap->SetName(string_to_wstring("CBV/SRV/UAV Descriptor Heap").c_str()));
}

void GraphicsResourceDescriptorHeap::clean_up() {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) descriptor_heap.Reset();
	occupation.clear();
}

UINT GraphicsResourceDescriptorHeap::get_open_heap_index() {
	auto find = std::ranges::find(occupation, false);
	if (find != occupation.end()) return (UINT)std::distance(occupation.begin(), find);
	else return (UINT)occupation.size();
}

void GraphicsResourceDescriptorHeap::create_cbv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &upload_buffer, size_t obj_size, UINT heap_index) {
	D3D12_CONSTANT_BUFFER_VIEW_DESC view_desc = { };
	view_desc.BufferLocation = upload_buffer->GetGPUVirtualAddress();
	view_desc.SizeInBytes = (obj_size + 255) & ~255; // CB size is required to be 256-byte aligned.

	D3D12_CPU_DESCRIPTOR_HANDLE handle = { };
	handle.ptr = descriptor_heap->GetCPUDescriptorHandleForHeapStart().ptr + heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateConstantBufferView(&view_desc, handle);

	reserve_descriptor_index(heap_index);
}

void GraphicsResourceDescriptorHeap::create_srv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &default_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc, UINT heap_index) {
	// Taking the first descriptor of the heap as a simple example  
	D3D12_CPU_DESCRIPTOR_HANDLE handle = { };
	handle.ptr = descriptor_heap->GetCPUDescriptorHandleForHeapStart().ptr + heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); 
	device->CreateShaderResourceView(default_buffer.Get(), &srv_desc, handle);

	reserve_descriptor_index(heap_index);
}

void GraphicsResourceDescriptorHeap::remove_descriptor(UINT heap_index) {
	occupation[heap_index] = false;
}

void GraphicsResourceDescriptorHeap::reserve_descriptor_index(UINT heap_index) {
	if (heap_index >= occupation.size()) {
		occupation.resize(heap_index+1);
	}
	occupation[heap_index] = true;
}

