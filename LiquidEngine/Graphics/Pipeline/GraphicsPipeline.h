#pragma once

#include <iostream>
#include <queue>
#include "GraphicsPipelineRootArgumentContainers.h"
#include "GraphicsPipelineIACommand.h"
#include "Storage.h"

#define ZeroStruct(STRUCT) ZeroMemory(STRUCT, sizeof(STRUCT))

/**
* D3D12 graphics pipeline wrapper.
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

	void compile() { compile_signal = true; }

	void clean_up();

	bool operator==(const GraphicsPipeline &pipeline) const noexcept;

	ComPtr<ID3D12PipelineState> pipeline_state_object = nullptr; // pso containing a pipeline state

	bool compile_signal = true;

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = {
		{ "POSITION",			0u,	DXGI_FORMAT_R32G32B32_FLOAT,	0u,	0u,								D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		0u },
		{ "TEXCOORD",			0u,	DXGI_FORMAT_R32G32_FLOAT,		0u,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		0u },
		{ "NORMAL",				0u,	DXGI_FORMAT_R32G32B32_FLOAT,	0u,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		0u },
		{ "TANGENT",			0u,	DXGI_FORMAT_R32G32B32_FLOAT,	0u,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		0u },
		{ "INSTANCE_POSITION",	0u, DXGI_FORMAT_R32G32B32_FLOAT,	1u, 0u,								D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,	1u },
		{ "INSTANCE_ROTATION",	0u, DXGI_FORMAT_R32G32B32_FLOAT,	1u, D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,	1u },
		{ "INSTANCE_SIZE",		0u, DXGI_FORMAT_R32G32B32_FLOAT,	1u, D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,	1u },
	};

	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	/**
	 * Uploads mesh data.
	 * \see Mesh
	 * \see GraphicsPipelineMeshChange::Manager
	 */
	class InputAssembler {
	public:
		InputAssembler() { }

		void set_instances(const std::vector<Transform>& instances, const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list);

		void add_mesh(const Mesh &mesh, const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index = -1);
		void remove_mesh(size_t index);
		void remove_all_meshes();

		void check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list);
		void run(const ComPtr<ID3D12GraphicsCommandList> &command_list);

		void draw_meshes(const ComPtr<ID3D12GraphicsCommandList> &command_list);

		void clean_up();

		GET const std::vector<D3D12_VERTEX_BUFFER_VIEW> & get_vertex_buffer_views() const noexcept;
		GET D3D12_VERTEX_BUFFER_VIEW get_instance_buffer_view() const noexcept;

		void clear_commands() noexcept { commands = std::queue<std::shared_ptr<GraphicsPipelineIACommand>>{}; }

		void add_command(const std::shared_ptr<GraphicsPipelineIACommand>&& command) noexcept {
			commands.push(command);
		}

		bool operator==(const InputAssembler &input_assembler) const noexcept {
			return (
				vertex_buffers == input_assembler.vertex_buffers &&
				vertex_buffer_views == input_assembler.vertex_buffer_views &&
				primitive_topology_type == input_assembler.primitive_topology_type &&
				primitive_topology == input_assembler.primitive_topology
			);
		}

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

	} input_assembler;

	std::string vs{"Graphics/DefaultVertex.hlsl"};
	std::string hs{};
	std::string ds{};
	std::string gs{};
	std::string ps{"Graphics/LitPixel.hlsl"};

	/**
	* Rasterizer stage.
	*/
	class Rasterizer {
	public:
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

		D3D12_RASTERIZER_DESC desc{CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT)};
	} rasterizer;

	/**
	* Stream output stage.
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
	* Uploads data and structures to shaders.
	*/
	class RootSignature { // https://learn.microsoft.com/en-us/windows/win32/direct3d12/pipelines-and-shaders-with-directx-12
	public:
		RootSignature() { }

		void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps);
		
		void clean_up();

		void check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps);
		
		void run(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps);

		bool operator==(const RootSignature &root_signature) const noexcept;
		
		/**
		* Binds root constants to root signature. Uses the RootConstants class from the container to bind.
		* \param obj Root constants container to bind.
		* \param shader Shader the root constants will be used in.
		* \param number of root constants to bind.
		*/
		void bind_shader_resource_view(std::shared_ptr<ShaderResourceView>& srv, D3D12_SHADER_VISIBILITY shader);

		/**
		 * Binds root constants to root signature.
		 * \param rc Root constants to be bound.
		 * \param shader Shader the root constants will be used in.
		 * \param number of root constants to bind.
		 */
		void bind_root_constants(std::shared_ptr<RootConstants>& rc, D3D12_SHADER_VISIBILITY shader, UINT number_of_values = -1) {
			UINT index = (UINT)constant_buffers.size() + (UINT)root_constants.size();
			rc->compile(shader, index + (UINT)shader_resource_views.size(), index, number_of_values);
			root_constants.push_back(rc);
		}

		/**
		* Binds root constants to root signature. Uses the RootConstants class from the container to bind.
		* \param obj Root constants container to bind.
		* \param shader Shader the root constants will be used in.
		* \param number of root constants to bind.
		*/
		template <typename T>
		void bind_root_constants(RootConstantsContainer<T>& obj, D3D12_SHADER_VISIBILITY shader, UINT number_of_values = -1) {
			bind_root_constants(obj.rc, shader, number_of_values);
		}

		/**
		* Binds a constant buffer to root signature.
		* \param rc Constant buffer to be bound.
		* \param shader Shader the constant buffer will be used in.
		*/
		void bind_constant_buffer(std::shared_ptr<ConstantBuffer>& cb, D3D12_SHADER_VISIBILITY shader);

		/**
		* Binds a constant buffer to root signature. Uses the ConstantBuffer class from the container to bind.
		* \param obj Constant buffer container to bind.
		* \param shader Shader the root constants will be used in.
		*/
		template <typename T>
		void bind_constant_buffer(ConstantBufferContainer<T>& cb, D3D12_SHADER_VISIBILITY shader) {
			bind_constant_buffer(cb.cb, shader);
		}

		void create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps);

		GET const std::vector<std::weak_ptr<ConstantBuffer>> & get_constant_buffers() const noexcept { return constant_buffers; }
		GET const std::vector<std::weak_ptr<RootConstants>> & get_root_constants() const noexcept { return root_constants; }
		GET const std::vector<std::weak_ptr<ShaderResourceView>> & get_shader_resource_views() const noexcept { return shader_resource_views; }

		GET const std::vector<std::weak_ptr<DescriptorTable>> & get_descriptor_tables() const noexcept { return descriptor_tables; }
		GET const std::vector<D3D12_ROOT_PARAMETER> & get_root_params() const noexcept { return compilation_params; }

	private:
		friend GraphicsPipeline;

		std::vector<std::weak_ptr<DescriptorTable>> descriptor_tables{};

		std::vector<std::weak_ptr<ConstantBuffer>> constant_buffers{};
		std::vector<std::weak_ptr<RootConstants>> root_constants{};
		std::vector<std::weak_ptr<ShaderResourceView>> shader_resource_views{};

		CD3DX12_ROOT_SIGNATURE_DESC signature_desc{};

		std::vector<D3D12_ROOT_PARAMETER> compilation_params{};

		ComPtr<ID3D12RootSignature> signature{nullptr}; // Root signature defines data shaders will access
	} root_signature;
};
