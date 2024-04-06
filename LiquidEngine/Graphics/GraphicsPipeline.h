#pragma once

#pragma warning(push)
#pragma warning(disable : 26495)
#include "d3dx12.h"
#pragma warning(pop)
#include <DirectXTex.h>
#include <iostream>
#include "GraphicsPipelineMeshChange.h"
#include "ResourceManager.h"
#include "ShaderStorage.h"
#include "Renderer/GraphicsDescriptorHeaps.h"

#define ZeroStruct(STRUCT) ZeroMemory(STRUCT, sizeof(STRUCT))

/**
* D3D12 graphics pipeline wrapper.
*/
class GraphicsPipeline {
public:
	GraphicsPipeline() { }

	void check_for_update(const ComPtr<ID3D12Device> &device,
		const ComPtr<ID3D12GraphicsCommandList> &command_list,
		int frame_index, GraphicsDescriptorHeaps &descriptor_heaps);
	
	void run(const ComPtr<ID3D12Device> &device,
		const ComPtr<ID3D12GraphicsCommandList> &command_list,
		int frame_index, GraphicsDescriptorHeaps &descriptor_heaps);
	
	void draw(const ComPtr<ID3D12GraphicsCommandList> &command_list);

	void compile(const ComPtr<ID3D12Device> &device,
		const ComPtr<ID3D12GraphicsCommandList> &command_list,
		const DXGI_SAMPLE_DESC &sample_desc,
		const D3D12_BLEND_DESC &blend,
		GraphicsDescriptorHeaps &descriptor_heaps);

	void clean_up();

	bool operator==(const GraphicsPipeline &pipeline) const noexcept;
	void operator=(const GraphicsPipeline &pipeline) noexcept;

	ComPtr<ID3D12PipelineState> pipeline_state_object = nullptr; // pso containing a pipeline state

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = {
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,								D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 }
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

		void add_mesh(const Mesh &mesh, const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index = -1);
		void remove_mesh(size_t index);
		void remove_all_meshes();

		void check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list);
		void run(const ComPtr<ID3D12GraphicsCommandList> &command_list);

		void clean_up();

		void set_proxy(const std::shared_ptr<GraphicsPipelineMeshChange::Manager> &change_manager) {
			this->change_manager = change_manager;
		}

		const std::vector<D3D12_VERTEX_BUFFER_VIEW> & get_vertex_buffer_views() const noexcept;

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
		
		std::shared_ptr<GraphicsPipelineMeshChange::Manager> change_manager = nullptr;

		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		D3D12_PRIMITIVE_TOPOLOGY primitive_topology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		std::vector<ComPtr<ID3D12Resource>> vertex_buffers; // a default buffer in GPU memory that we will load vertex data for our triangle into
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views; // a structure containing a pointer to the vertex data in gpu memory
																   // the total size of the buffer, and the size of each element (vertex)
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
		Rasterizer() { }

		bool operator==(const Rasterizer &rasterizer) const noexcept {
			return (desc == rasterizer.desc);
		}

		void set_desc(const D3D12_RASTERIZER_DESC &desc) noexcept {
			this->desc = desc;
		}

		D3D12_RASTERIZER_DESC get_desc() const noexcept {
			return desc;
		}

	private:
		friend GraphicsPipeline;

		D3D12_RASTERIZER_DESC desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	} rasterizer;

	/**
	* Stream output stage.
	*/
	class StreamOutput {
	public:
		StreamOutput() : desc({}) { }
		StreamOutput(const D3D12_STREAM_OUTPUT_DESC &desc)
			: desc(desc) { }

		bool operator==(const StreamOutput &stream_output) const noexcept {
			return (desc == stream_output.desc);
		}

		D3D12_STREAM_OUTPUT_DESC get_desc() const noexcept {
			return desc;
		}

	private:
		friend GraphicsPipeline;

		D3D12_STREAM_OUTPUT_DESC desc = {};
	} stream_output;

	/**
	* Uploads data and structures to shaders.
	*/
	class RootSignature { // https://learn.microsoft.com/en-us/windows/win32/direct3d12/pipelines-and-shaders-with-directx-12
	public:
		/**
		 * Root argument to be bound to pipeline.
		 */
		class RootArgument {
		public:
			RootArgument() { }
			RootArgument(UINT parameter_index);

			UINT get_parameter_index() { return parameter_index; }

		protected:
			friend RootSignature;

			std::vector<D3D12_ROOT_PARAMETER> root_parameters = { };

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
			
			bool operator==(const DescriptorTable &descriptor_table) const noexcept;

			const D3D12_ROOT_DESCRIPTOR_TABLE & get_table() const noexcept { return table; }
			const std::vector<CD3DX12_DESCRIPTOR_RANGE> & get_ranges() const noexcept { return ranges; }
		private:
			friend RootSignature;

			D3D12_ROOT_DESCRIPTOR_TABLE table;

			UINT heap_index{(UINT)-1};
			
			std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges = { }; // In practice you often only have one descriptor range per-table.
		};

		/**
		* Root constants.
		*/
		class RootConstants : public RootArgument {
		public:
			RootConstants() { }

			template <typename T>
			RootConstants(T &obj, D3D12_SHADER_VISIBILITY shader, UINT index, UINT parameter_index, UINT number_of_values = -1) {
				compile<T>(obj, shader, index, number_of_values);
			}

			template <typename T>
			void set_obj(T *obj) {
				this->obj = static_cast<void*>(obj);
				obj_size = sizeof(obj);
			}

			void compile(D3D12_SHADER_VISIBILITY shader, UINT parameter_index, UINT index, UINT number_of_values = -1) {
				this->parameter_index = parameter_index;
				this->root_parameters.resize(1u);
				
				constants.Num32BitValues = (number_of_values == (UINT)-1 ? (UINT)std::ceilf((float)obj_size*8.0f / 32.0f) : number_of_values);
				constants.RegisterSpace = 0u;
				constants.ShaderRegister = index;

				root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
				root_parameters[0].Constants = constants;
				root_parameters[0].ShaderVisibility = shader;
			}

			template <typename T>
			void compile(T &obj, D3D12_SHADER_VISIBILITY shader, UINT index, UINT number_of_values = -1) {
				set_obj<T>(obj);
				compile(shader, index, number_of_values);
			}

			const D3D12_ROOT_CONSTANTS & get_constants() const noexcept { return constants; }

		private:
			friend RootSignature;
			
			void* obj = nullptr;
			size_t obj_size{0u};

			D3D12_ROOT_CONSTANTS constants = { };
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

			virtual void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsDescriptorHeaps &descriptor_heaps) = 0;
			void clean_up();
			
			virtual void create_views(const ComPtr<ID3D12Device> &device, GraphicsDescriptorHeaps &descriptor_heaps) = 0;

			/** Returns true if object is eligible for resource creation. */
			virtual bool valid() { return heap_index != (UINT)-1; };

			std::shared_ptr<DescriptorTable> descriptor_table{nullptr};
			
			bool update_signal{false};

		protected:
			UINT heap_index{(UINT)-1};

			ComPtr<ID3D12Resource> default_heap{nullptr};
		};

		/**
		* Constant buffer.
		*/
		class ConstantBuffer : public DescriptorRootObject {
		public:
			ConstantBuffer() : DescriptorRootObject() { }

			template <typename T>
			ConstantBuffer(T &cb, std::string name = "")
				: obj(static_cast<void*>(&cb)), obj_size(sizeof(T)),
				name(name.empty() ? typeid(T).name() : name) { }

			void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsDescriptorHeaps &descriptor_heaps) override;
			void update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list);
			
			void create_views(const ComPtr<ID3D12Device> &device, GraphicsDescriptorHeaps &descriptor_heaps) override;

			bool valid() override { return (DescriptorRootObject::valid() && obj_size != 0); }

			bool operator==(const ConstantBuffer &cb) const noexcept;

			mutable std::string name{};

			void* obj = nullptr;
			size_t obj_size = 0u;
		};

		/**
		* Shader resource view. Used for uploading texture data.
		*/
		class ShaderResourceView : public DescriptorRootObject {
		public:
			ShaderResourceView() : DescriptorRootObject() { }
			ShaderResourceView(const DirectX::ScratchImage &mip_chain, bool is_texture_cube = false);

			void update_descs(const DirectX::ScratchImage &mip_chain, bool is_texture_cube = false);

			void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsDescriptorHeaps &descriptor_heaps) override;
			void compile() { update_signal = true; }

			void create_views(const ComPtr<ID3D12Device> &device, GraphicsDescriptorHeaps &descriptor_heaps) override;
			
			bool valid() override { return (DescriptorRootObject::valid() && heap_desc.Width != 0 && heap_desc.Height != 0); }

			D3D12_RESOURCE_DESC heap_desc{};

			std::vector<D3D12_SUBRESOURCE_DATA> subresources{};

			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
		};

		/**
		* Stores a structure and the constant buffer. The structure is the data to be written to the constant buffer. Not required, but useful.
		* \see GraphicsPipeline::RootSignature::ConstantBuffer
		*/
		template <typename T>
		class ConstantBufferContainer {
		public:
			ConstantBufferContainer() { }

			ConstantBufferContainer(const T &obj)
				: obj(std::make_shared<T>(obj)) {
				cb = GraphicsPipeline::RootSignature::ConstantBuffer(*this->obj);
			}

			void clean_up() {
				obj.reset();
			}

			void update() { cb.update_signal = true; }

			bool operator==(ConstantBufferContainer &c) {
				return (obj == c.obj);
			}

			std::shared_ptr<T> obj = { };
			GraphicsPipeline::RootSignature::ConstantBuffer cb;
		};

		/**
		* Stores a structure and the root constants. The structure is the data to be written to the root constants. Not required, but useful.
		*/
		template <typename T>
		class RootConstantsContainer {
		public:
			RootConstantsContainer() { }

			RootConstantsContainer(const T &obj)
				: obj(std::make_shared<T>(obj)) {
				rc = GraphicsPipeline::RootSignature::RootConstants();
				rc.set_obj<T>(this->obj.get());
			}

			void clean_up() {
				obj.reset();
			}

			std::shared_ptr<T> obj = { };
			GraphicsPipeline::RootSignature::RootConstants rc;
		};

		RootSignature() { }

		void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsDescriptorHeaps &descriptor_heaps);
		
		void clean_up();

		void check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index, GraphicsDescriptorHeaps &descriptor_heaps);
		
		void run(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index, GraphicsDescriptorHeaps &descriptor_heaps);

		bool operator==(const RootSignature &root_signature) const noexcept;
		
		/**
		* Binds root constants to root signature. Uses the RootConstants class from the container to bind.
		* \param obj Root constants container to bind.
		* \param shader Shader the root constants will be used in.
		* \param number of root constants to bind.
		*/
		void bind_shader_resource_view(ShaderResourceView &srv, D3D12_SHADER_VISIBILITY shader);

		/**
		 * Binds root constants to root signature.
		 * \param rc Root constants to be bound.
		 * \param shader Shader the root constants will be used in.
		 * \param number of root constants to bind.
		 */
		void bind_root_constants(RootConstants &rc, D3D12_SHADER_VISIBILITY shader, UINT number_of_values = -1) {
			UINT index = (UINT)constant_buffers.size() + (UINT)root_constants.size();
			rc.compile(shader, index + (UINT)shader_resource_views.size(), index, number_of_values);
			root_constants.push_back(&rc);
		}

		/**
		* Binds root constants to root signature. Uses the RootConstants class from the container to bind.
		* \param obj Root constants container to bind.
		* \param shader Shader the root constants will be used in.
		* \param number of root constants to bind.
		*/
		template <typename T>
		void bind_root_constants(RootConstantsContainer<T> &obj, D3D12_SHADER_VISIBILITY shader, UINT number_of_values = -1) {
			bind_root_constants(obj.rc, shader, number_of_values);
		}

		/**
		* Binds a constant buffer to root signature.
		* \param rc Constant buffer to be bound.
		* \param shader Shader the constant buffer will be used in.
		*/
		void bind_constant_buffer(ConstantBuffer &cb, D3D12_SHADER_VISIBILITY shader);

		/**
		* Binds a constant buffer to root signature. Uses the ConstantBuffer class from the container to bind.
		* \param obj Constant buffer container to bind.
		* \param shader Shader the root constants will be used in.
		*/
		template <typename T>
		void bind_constant_buffer(ConstantBufferContainer<T> &cb, D3D12_SHADER_VISIBILITY shader) {
			bind_constant_buffer(cb.cb, shader);
		}

		void create_views(const ComPtr<ID3D12Device> &device, GraphicsDescriptorHeaps &descriptor_heaps);

		const std::vector<ConstantBuffer*> & get_constant_buffers() const noexcept { return constant_buffers; }
		const std::vector<RootConstants*> & get_root_constants() const noexcept { return root_constants; }
		const std::vector<ShaderResourceView*> & get_shader_resource_views() const noexcept { return shader_resource_views; }

		const std::vector<std::shared_ptr<DescriptorTable>> & get_descriptor_tables() const noexcept { return descriptor_tables; }
		const std::vector<D3D12_ROOT_PARAMETER> & get_root_params() const noexcept { return compilation_params; }

	private:
		friend GraphicsPipeline;

		std::vector<std::shared_ptr<DescriptorTable>> descriptor_tables = { };

		std::vector<ConstantBuffer*> constant_buffers = { };
		std::vector<RootConstants*> root_constants = { };
		std::vector<ShaderResourceView*> shader_resource_views = { };

		CD3DX12_ROOT_SIGNATURE_DESC signature_desc = { };

		std::vector<D3D12_ROOT_PARAMETER> compilation_params = { };

		ComPtr<ID3D12RootSignature> signature = nullptr; // Root signature defines data shaders will access
	} root_signature;
};
