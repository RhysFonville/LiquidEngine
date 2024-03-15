#pragma once

#include "../globalutil.h"
#include "../Debug/Throw.h"

static constexpr UINT NUMBER_OF_BUFFERS = 3u;

class GraphicsDescriptorHeaps {
public:
	GraphicsDescriptorHeaps();

	void compile(const ComPtr<ID3D12Device2> &device);

	ComPtr<ID3D12DescriptorHeap> & operator[](UINT i) { return descriptor_heaps[i]; }

	UINT get_next_heap_index();
	void increment_heap_index();

	void create_cbv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &upload_buffer, size_t obj_size, UINT frame_index, UINT heap_index);
	void create_srv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &default_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc, UINT frame_index, UINT heap_index);

private:
	ComPtr<ID3D12DescriptorHeap> descriptor_heaps[NUMBER_OF_BUFFERS] = { }; // Stores the descriptor to our constant buffer
	UINT next_heap_index = 0u;
};

