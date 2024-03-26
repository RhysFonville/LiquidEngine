#include "GraphicsDescriptorHeaps.h"

GraphicsDescriptorHeaps::GraphicsDescriptorHeaps() {
	
}

void GraphicsDescriptorHeaps::compile(const ComPtr<ID3D12Device2> &device) {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
		heap_desc.NumDescriptors = 1000000; // 100,000 descriptors -> 0.4 megabytes
		heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heap_desc.NodeMask = 0u;

		HPEW(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&descriptor_heaps[i])));
		HPEW(descriptor_heaps[i]->SetName(string_to_wstring("CBV/SRV/UAV Descriptor Heap #" + std::to_string(i)).c_str()));
	}
}

UINT GraphicsDescriptorHeaps::get_next_heap_index() {
	return next_heap_index;
}

void GraphicsDescriptorHeaps::increment_heap_index() {
	next_heap_index++;
}

void GraphicsDescriptorHeaps::create_cbv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &upload_buffer, size_t obj_size, UINT frame_index, UINT heap_index) {
	D3D12_CONSTANT_BUFFER_VIEW_DESC view_desc = { };
	view_desc.BufferLocation = upload_buffer->GetGPUVirtualAddress();
	view_desc.SizeInBytes = (obj_size + 255) & ~255; // CB size is required to be 256-byte aligned.

	D3D12_CPU_DESCRIPTOR_HANDLE handle = { };
	handle.ptr = descriptor_heaps[frame_index]->GetCPUDescriptorHandleForHeapStart().ptr + heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateConstantBufferView(&view_desc, handle);
}

void GraphicsDescriptorHeaps::create_srv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &default_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc, UINT frame_index, UINT heap_index) {
	// Taking the first descriptor of the heap as a simple example  
	D3D12_CPU_DESCRIPTOR_HANDLE handle = { };
	handle.ptr = descriptor_heaps[frame_index]->GetCPUDescriptorHandleForHeapStart().ptr + heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); 
	device->CreateShaderResourceView(default_buffer.Get(), &srv_desc, handle);
}
