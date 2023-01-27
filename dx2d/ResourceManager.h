#pragma once

#define NOMINMAX

#include <Windows.h>
#include <cmath>
#include <utility>
#include "globalutil.h"
#include "d3dx12.h"
#include "Throw.h"
#include "MeshComponent.h"

using VertexBuffer = std::tuple<ComPtr<ID3D12Resource>, D3D12_VERTEX_BUFFER_VIEW, std::shared_ptr<MeshComponent>>;

class ResourceManager {
public:
	ResourceManager();

	void create_depth_stencil(const UVector2 &resolution, ComPtr<ID3D12Device> &device);

	void cleanup();

	class StaticMeshes {
	public:
		void add_mesh(const std::shared_ptr<MeshComponent> &mesh, ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index = -1);
		void remove_mesh(size_t index);

		const std::vector<VertexBuffer> & get_vertex_buffers() const noexcept;

	private:
		friend ResourceManager;

		std::vector<VertexBuffer> vertex_buffers; // a default buffer in GPU memory that we will load vertex data for our triangle into
												  // the vertex buffer view is a structure containing a pointer to the vertex data in gpu memory
												  // the total size of the buffer, and the size of each element (vertex)
	} static_meshes;

	ComPtr<ID3D12Resource> depth_stencil_buffer = nullptr; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ComPtr<ID3D12DescriptorHeap> depth_stencil_descriptor_heap = nullptr; // This is a heap for our depth/stencil buffer descriptor
};

