#pragma once

#include "../../Utility/commonmacros.h"
#include "../Debug/Throw.h"

static constexpr UINT NUMBER_OF_BUFFERS = 3u;

class GraphicsResourceDescriptorHeap {
public:
	GraphicsResourceDescriptorHeap();

	void compile(const ComPtr<ID3D12Device> &device);

	void clean_up();

	//ComPtr<ID3D12DescriptorHeap> & operator[](UINT i) { return descriptor_heaps[i]; }
	ComPtr<ID3D12DescriptorHeap>& get() {
		return descriptor_heap;
	}

	GET UINT get_open_heap_index();

	void create_cbv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &upload_buffer, size_t obj_size, UINT frame_index, UINT heap_index);
	void create_srv(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12Resource> &default_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc, UINT frame_index, UINT heap_index);

	void remove_descriptor(UINT heap_index);

	void reserve_descriptor_index(UINT heap_index);

private:
	ComPtr<ID3D12DescriptorHeap> descriptor_heap = { }; // Stores the descriptor to our constant buffer
	std::vector<bool> occupation;
};

