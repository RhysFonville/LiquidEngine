#pragma once

#include <iostream>
#include <queue>
#include <ranges>
#include <map>
#include "GraphicsPipelineIACommand.h"
#include "Storage.h"
#include "GraphicsPipelineRootArguments.h"

#define ZeroStruct(STRUCT) ZeroMemory(STRUCT, sizeof(STRUCT))

#define HPEW_ERR_BLOB_PARAM(buf) ((buf == nullptr ? "" : (char*)buf->GetBufferPointer()))

/**
* D3D12 graphics pipeline wrapper.
* 
*  <a href="https://learn.microsoft.com/en-us/windows/win32/direct3d12/pipelines-and-shaders-with-directx-12">Pipeline</a>
*/
class GraphicsPipeline {
public:
	GraphicsPipeline() { }

	void check_for_update(const ComPtr<ID3D12Device> &device,
		const ComPtr<ID3D12GraphicsCommandList> &command_list,
		const DXGI_SAMPLE_DESC &sample_desc,
		const D3D12_BLEND_DESC &blend_desc,
		GraphicsResourceDescriptorHeap &descriptor_heaps);

	void run(const ComPtr<ID3D12Device> &device,
		const ComPtr<ID3D12GraphicsCommandList> &command_list,
		const DXGI_SAMPLE_DESC &sample_desc,
		const D3D12_BLEND_DESC &blend_desc,
		GraphicsResourceDescriptorHeap &descriptor_heaps);
	
	void draw(const ComPtr<ID3D12GraphicsCommandList> &command_list);

	void compile(const ComPtr<ID3D12Device> &device,
		const ComPtr<ID3D12GraphicsCommandList> &command_list,
		const DXGI_SAMPLE_DESC &sample_desc,
		const D3D12_BLEND_DESC &blend,
		GraphicsResourceDescriptorHeap &descriptor_heaps);

	void compile() { 
		compile_signal = true;
	}

	void clean_up();

	bool operator==(const GraphicsPipeline &pipeline) const noexcept;

	ComPtr<ID3D12PipelineState> pipeline_state_object = nullptr; // pso containing a pipeline state

	bool compile_signal = true;

	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	/**
	 * Input assembler stage.
	 * \see Mesh
	 * \see GraphicsPipelineIACommand
	 */
	class InputAssembler {
	public:
		InputAssembler() { }

		bool operator==(const InputAssembler& input_assembler) const noexcept;

		void compile(const std::weak_ptr<Shader>& vs);

		//void set_instances(const std::vector<Transform>& instances, const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list);

		void add_mesh(const Mesh &mesh, const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index = -1);
		void remove_mesh(size_t index);
		void remove_all_meshes();

		void check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list);
		void run(const ComPtr<ID3D12GraphicsCommandList> &command_list);

		void draw_meshes(const ComPtr<ID3D12GraphicsCommandList> &command_list);

		void clean_up();

		GET const std::vector<D3D12_VERTEX_BUFFER_VIEW> & get_vertex_buffer_views() const noexcept;
		//GET D3D12_VERTEX_BUFFER_VIEW get_instance_buffer_view() const noexcept;

		void clear_commands() noexcept;
		void add_command(const std::shared_ptr<GraphicsPipelineIACommand>&& command) noexcept;

		const std::vector<D3D12_INPUT_ELEMENT_DESC>& get_input_layout() const noexcept;
		void set_input_layout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& layout) noexcept;

		D3D12_INPUT_LAYOUT_DESC get_input_layout_desc() const noexcept;

	private:
		friend GraphicsPipeline;
		
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		D3D12_PRIMITIVE_TOPOLOGY primitive_topology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		std::vector<ComPtr<ID3D12Resource>> vertex_buffers{}; // a default buffer in GPU memory that we will load vertex data for our triangle into
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views{}; // a structure containing a pointer to the vertex data in gpu memory
																   // the total size of the buffer, and the size of each element (vertex)
		ComPtr<ID3D12Resource> instance_buffer{};
		D3D12_VERTEX_BUFFER_VIEW instance_buffer_view{};
		
		std::queue<std::shared_ptr<GraphicsPipelineIACommand>> commands;

		std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = {
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
			{"TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
			{"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
			{"TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u},
			//{"INSTANCE_POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 1u, 0u, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1u},
			//{"INSTANCE_ROTATION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 1u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1u},
			//{"INSTANCE_SIZE", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 1u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1u},
		};

	} input_assembler;
	
	struct Shaders {
		std::weak_ptr<Shader> vs{};
		std::weak_ptr<Shader> hs{};
		std::weak_ptr<Shader> ds{};
		std::weak_ptr<Shader> gs{};
		std::weak_ptr<Shader> ps{};
	} shaders;

	/**
	* Rasterizer stage.
	*/
	class Rasterizer {
	public:
		Rasterizer() { }
		Rasterizer(const D3D12_RASTERIZER_DESC& desc) : desc{desc} { }

		bool operator==(const Rasterizer &rasterizer) const noexcept {
			return (desc == rasterizer.desc);
		}

		void set_desc(const D3D12_RASTERIZER_DESC &desc) noexcept {
			this->desc = desc;
		}

		GET D3D12_RASTERIZER_DESC get_desc() const noexcept {
			return desc;
		}

	private:
		friend GraphicsPipeline;

		D3D12_RASTERIZER_DESC desc{CD3DX12_RASTERIZER_DESC{D3D12_DEFAULT}};
	} rasterizer;

	/**
	* Stream output of the pipeline.
	*/
	class StreamOutput {
	public:
		StreamOutput() { }
		StreamOutput(const D3D12_STREAM_OUTPUT_DESC &desc) : desc{desc} { }

		bool operator==(const StreamOutput &stream_output) const noexcept {
			return (desc == stream_output.desc);
		}

		GET D3D12_STREAM_OUTPUT_DESC get_desc() const noexcept {
			return desc;
		}

	private:
		friend GraphicsPipeline;

		D3D12_STREAM_OUTPUT_DESC desc{};
	} stream_output;

	/**
	* Root signature of the pipeline. Uploads data and structures to shaders.
	*/
	class RootSignature {
	public:
		RootSignature() { }

		bool operator==(const RootSignature& root_signature) const noexcept;

		void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps, const Shaders& shaders);
		void compile_resources(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heaps);

		void bind_shader_resource_parameters(const std::weak_ptr<Shader>& s);

		void clean_up();

		void check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps);
		
		void run(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps);

		void create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps);

		GET const std::map<std::string, std::shared_ptr<ConstantBuffer>> & get_constant_buffers() const noexcept { return constant_buffers; }
		//GET const std::vector<std::shared_ptr<RootConstants>> & get_root_constants() const noexcept { return root_constants; }
		GET const std::map<std::string, std::shared_ptr<ShaderResourceView>> & get_shader_resource_views() const noexcept { return shader_resource_views; }

		GET std::weak_ptr<DescriptorRootObject> get_resource(const std::string& name) const;
		GET std::weak_ptr<ConstantBuffer> get_constant_buffer(const std::string& name) const;
		GET std::weak_ptr<ShaderResourceView> get_shader_resource_view(const std::string& name) const;

		GET std::vector<DescriptorTable> get_descriptor_tables() const noexcept;
		GET std::vector<D3D12_ROOT_PARAMETER1> get_root_params() const noexcept;

	private:
		friend GraphicsPipeline;

		std::vector<const DescriptorTable*> descriptor_tables{};

		std::map<std::string, std::shared_ptr<ConstantBuffer>> constant_buffers{};
		//std::vector<std::shared_ptr<RootConstants>> root_constants{};
		std::map<std::string, std::shared_ptr<ShaderResourceView>> shader_resource_views{};

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC signature_desc{};

		ComPtr<ID3D12RootSignature> signature{nullptr}; // Root signature defines data shaders will access
	} root_signature;
};
