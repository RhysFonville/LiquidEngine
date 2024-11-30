#pragma once

#pragma warning(push)
#pragma warning(disable : 26495)
#include "../d3dx12.h"
#pragma warning(pop)

#include <string>
#include <vector>
#include <DirectXTex.h>
#include <ranges>
#include "../GraphicsResourceDescriptorHeap.h"
#include "../ResourceManager.h"

/**
* Root argument to be bound to pipeline.
*/
class RootArgument {
public:
	RootArgument() { }
	RootArgument(UINT parameter_index);

	GET UINT get_parameter_index() const noexcept;
	GET const std::vector<D3D12_ROOT_PARAMETER>& get_root_parameters() const noexcept;

protected:
	std::vector<D3D12_ROOT_PARAMETER> root_parameters{};
	UINT parameter_index{(UINT)-1};
};

/**
* Descriptor table to describe data to be uploaded.
*/
class DescriptorTable : public RootArgument {
public:
	DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index, UINT parameter_index);
	
	/*DescriptorTable(const DescriptorTable &t) {
		table = t.table;
		for (auto i = 0; i < RANGES_SIZE; i++) {
			ranges[i] = t.ranges[i];
		}
		for (auto i = 0; i < PARAMS_SIZE; i++) {
			root_parameters[i] = t.root_parameters[i];
		}
	}*/

	void compile(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index);

	void clean_up();

	void set_descriptor_table(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap);

	bool operator==(const DescriptorTable &descriptor_table) const noexcept;

	GET const D3D12_ROOT_DESCRIPTOR_TABLE & get_table() const noexcept;
	GET const std::vector<CD3DX12_DESCRIPTOR_RANGE> & get_ranges() const noexcept;
	
	UINT heap_index{(UINT)-1};

private:
	D3D12_ROOT_DESCRIPTOR_TABLE table{};
	std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges{}; // In practice you often only have one descriptor range per-table.
};

/**
* Root constants.
*/
class RootConstants : public RootArgument {
public:
	RootConstants() : RootArgument{} { }

	template <typename T>
	RootConstants(T &obj, D3D12_SHADER_VISIBILITY shader, UINT index, UINT parameter_index, UINT number_of_values = -1) {
		compile<T>(obj, shader, index, number_of_values);
	}

	template <typename T>
	void set_obj(T *obj) {
		this->obj = static_cast<void*>(obj);
		obj_size = sizeof(obj);
	}

	void compile(D3D12_SHADER_VISIBILITY shader, UINT parameter_index, UINT index, UINT number_of_values = -1);

	template <typename T>
	void compile(T &obj, D3D12_SHADER_VISIBILITY shader, UINT index, UINT number_of_values = -1) {
		set_obj<T>(obj);
		compile(shader, index, number_of_values);
	}

	void clean_up();

	void set_constants(const ComPtr<ID3D12GraphicsCommandList>& command_list);

	bool operator==(const RootConstants& rc) const noexcept;

	GET const D3D12_ROOT_CONSTANTS & get_constants() const noexcept;

private:
	void* obj{nullptr};
	size_t obj_size{0u};

	D3D12_ROOT_CONSTANTS constants{};
};

/**
 * Root objects with descriptor tables.
 *
 * For constant buffers and shader resource views.
 *
 * \see ConstantBuffer
 * \see ShaderResourceView
 */
class DescriptorRootObject {
public:
	DescriptorRootObject() { }

	virtual void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) = 0;
	
	void clean_up();

	virtual void create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) = 0;

	/** Returns true if object is eligible for resource creation. */
	virtual bool valid() { return heap_index != (UINT)-1; };

	bool operator==(const DescriptorRootObject& dro) const noexcept {
		return (heap_index == dro.heap_index);
	}

	std::shared_ptr<DescriptorTable> descriptor_table{nullptr};

	bool compile_signal{false};

protected:
	UINT heap_index{(UINT)-1};

	ComPtr<ID3D12Resource> default_heap{nullptr};
};

/**
* Constant buffer.
*/
class ConstantBuffer : public DescriptorRootObject {
public:
	ConstantBuffer() : DescriptorRootObject{} { }

	template <typename T>
	ConstantBuffer(T &cb, std::string name = "")
		: obj(static_cast<void*>(&cb)), obj_size(sizeof(T)),
		name(name.empty() ? typeid(T).name() : name), DescriptorRootObject{} { }

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) override;
	void compile() { compile_signal = true; }

	void update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list);

	void create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) override;

	bool valid() override { return (DescriptorRootObject::valid() && obj_size != 0); }

	bool operator==(const ConstantBuffer &cb) const noexcept;

	mutable std::string name{};

	bool update_signal{false};

	void* obj{nullptr};
	size_t obj_size{0u};
};

/**
* Shader resource view. Used for uploading texture data.
*/
class ShaderResourceView : public DescriptorRootObject {
public:
	ShaderResourceView() : DescriptorRootObject() { }
	ShaderResourceView(const DirectX::ScratchImage &mip_chain, bool is_texture_cube = false);

	void update_descs(const DirectX::ScratchImage &mip_chain, bool is_texture_cube = false);

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) override;
	void compile() { compile_signal = true; }

	void create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) override;

	bool valid() override { return (DescriptorRootObject::valid() && heap_desc.Width != 0 && heap_desc.Height != 0); }

	bool operator==(const ShaderResourceView& srv) const noexcept;

	std::vector<D3D12_SUBRESOURCE_DATA> subresources{};
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	D3D12_RESOURCE_DESC heap_desc{};
};
