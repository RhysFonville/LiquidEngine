#pragma once

#include <initguid.h>
#include <d3d12.h>

#pragma warning(push)
#pragma warning(disable : 26495)
#include "../d3dx12.h"
#pragma warning(pop)

#include <string>
#include <vector>
#include <DirectXTex.h>
#include <ranges>
#include <d3d12shader.h>
#include "../GraphicsResourceDescriptorHeap.h"
#include "../ResourceManager.h"
#include "../graphicsoperatoroverloading.h"

/**
* Root argument to be bound to pipeline.
*/
class RootArgument {
public:
	RootArgument(UINT index);
	RootArgument() { }

	virtual void clean_up() = 0;

	GET virtual D3D12_ROOT_PARAMETER1 get_root_param() const noexcept = 0;
	GET UINT get_index() const noexcept;

protected:
	UINT index{};
};

/**
* Descriptor table to describe data to be uploaded.
*/
class DescriptorTable : public RootArgument {
public:
	DescriptorTable() { }
	DescriptorTable(const D3D12_DESCRIPTOR_RANGE1& range, UINT param_index);
	DescriptorTable(D3D12_SHADER_INPUT_BIND_DESC bind_desc, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT param_index);
	DescriptorTable(const DescriptorTable& dt);

	void clean_up() override { }
	
	D3D12_ROOT_PARAMETER1 get_root_param() const noexcept;

	void set_descriptor_table(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap) const;

	bool operator==(const DescriptorTable &descriptor_table) const noexcept;

	GET D3D12_ROOT_DESCRIPTOR_TABLE1 get_table() const noexcept;
	GET const D3D12_DESCRIPTOR_RANGE1& get_range() const noexcept;
	
private:
	D3D12_DESCRIPTOR_RANGE1 range{};
};

/**
* Root constants.
*/
/*class RootConstants : public RootArgument {
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

	void clean_up() override;

	void set_constants(const ComPtr<ID3D12GraphicsCommandList>& command_list);

	bool operator==(const RootConstants& rc) const noexcept;

	GET const D3D12_ROOT_CONSTANTS & get_constants() const noexcept;

private:
	void* obj{nullptr};
	size_t obj_size{0u};

	D3D12_ROOT_CONSTANTS constants{};
};*/

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
	DescriptorRootObject(const DescriptorTable& dt) : descriptor_table{dt} { }
	
	bool operator==(const DescriptorRootObject& dro) const noexcept;

	virtual void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) = 0;
	void compile() noexcept;
	bool will_compile() const noexcept;

	virtual void clean_up();

	virtual void create_view(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) = 0;

	DescriptorTable descriptor_table{};

protected:
	//UINT heap_index{(UINT)-1};

	bool compile_signal{false};

	ComPtr<ID3D12Resource> default_heap{nullptr};
};

/**
* Constant buffer.
*/
class ConstantBuffer : public DescriptorRootObject {
public:
	ConstantBuffer(const DescriptorTable& dt) : DescriptorRootObject{dt} { }

	template <typename T>
	void set_obj(T& cb) {
		obj = static_cast<void*>(&cb);
		obj_size = sizeof(T);
	}

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) override;

	void update(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list);
	void update() noexcept { update_signal = true; }
	bool will_update() const noexcept {  return update_signal; }

	void clean_up() override;

	void create_view(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) override;

	bool operator==(const ConstantBuffer &cb) const noexcept;

	void* obj{nullptr};
	size_t obj_size{0u};

private:
	bool update_signal{false};
};

/**
* Shader resource view. Used for uploading texture data.
*/
class ShaderResourceView : public DescriptorRootObject {
public:
	ShaderResourceView() : DescriptorRootObject{} { }
	ShaderResourceView(const DescriptorTable& dt) : DescriptorRootObject{dt} { }

	void update_descs(const DirectX::ScratchImage &mip_chain, bool is_texture_cube = false);

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) override;
	void compile() { compile_signal = true; }
	
	void clean_up() override;

	void create_view(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) override;

	bool operator==(const ShaderResourceView& srv) const noexcept;

	std::vector<D3D12_SUBRESOURCE_DATA> subresources{};
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	D3D12_RESOURCE_DESC heap_desc{};
};
