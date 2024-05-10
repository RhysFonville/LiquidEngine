#pragma once

#include "../globalutil.h"
#include "../Debug/Throw.h"

static constexpr UINT NUMBER_OF_BUFFERS = 3u;

class GraphicsDescriptorHeaps {
public:
	GraphicsDescriptorHeaps();

	void compile(const ComPtr<ID3D12Device> &device);

	void clean_up();

	ComPtr<ID3D12DescriptorHeap> & operator[](UINT i) { return descriptor_heaps[i]; }

	GET UINT get_open_heap_index();

	void create_cbv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &upload_buffer, size_t obj_size, UINT frame_index, UINT heap_index);
	void create_srv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &default_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc, UINT frame_index, UINT heap_index);

	void remove_descriptor(UINT heap_index);

	void reserve_descriptor_index(UINT heap_index);

private:
	ComPtr<ID3D12DescriptorHeap> descriptor_heaps[NUMBER_OF_BUFFERS] = { }; // Stores the descriptor to our constant buffer
	std::vector<bool> occupation;
};

