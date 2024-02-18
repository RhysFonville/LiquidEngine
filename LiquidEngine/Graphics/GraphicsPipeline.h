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

#define ZeroStruct(STRUCT) ZeroMemory(STRUCT, sizeof(STRUCT))

struct alignas(16) GenerateMipsCB {
	UINT src_mip_level; // Texture level of source mip
	UINT num_mip_levels; // Number of OutMips to write: [1-4]
	UINT src_dimension; // Width and height of the source texture are even or odd.
	bool is_srgb; // Must apply gamma correction to sRGB textures.
	FVector2 texel_size; // 1.0 / OutMip1.Dimensions
};

/**
* D3D12 graphics pipeline wrapper.
*/
class GraphicsPipeline {
public:
	GraphicsPipeline() { }
	GraphicsPipeline(const ComPtr<ID3D12Device> &device,
		const DXGI_SAMPLE_DESC &sample_desc,
		const UVector2 &resolution);

	void update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index);
	void run(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution);

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution);

	void clean_up();

	bool operator==(const GraphicsPipeline &pipeline) const noexcept;
	void operator=(const GraphicsPipeline &pipeline) noexcept;

	static constexpr UINT NUMBER_OF_BUFFERS = 3u;

	ComPtr<ID3D12PipelineState> pipeline_state_object = nullptr; // pso containing a pipeline state
	
	bool compilation_signal = true;

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = {
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,								D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 }
	};

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

		void update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list);

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

	std::string vs{"DefaultVertex.hlsl"};
	std::string hs{};
	std::string ds{};
	std::string gs{};
	std::string ps{"LitPixel.hlsl"};

	/**
	* Tesselator stage.
	*/
	class Tesselator {
	public:
		bool operator==(const Tesselator &tesselator) const noexcept { return true; }
	
	private:
		friend GraphicsPipeline;
	} tesselator;

	/**
	* Rasterizer stage.
	*/
	class Rasterizer {
	public:
		Rasterizer() { }
		Rasterizer(const UVector2 &resolution) {
			compile(resolution);
		}

		void set_viewing_settings(const UVector2 &resolution) {
			// Fill out the Viewport
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = (float)resolution.x;
			viewport.Height = (float)resolution.y;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			// Fill out a scissor rect
			scissor_rect.left = 0;
			scissor_rect.top = 0;
			scissor_rect.right = resolution.x;
			scissor_rect.bottom = resolution.y;
		}
		
		void compile(const UVector2 &resolution) {
			set_viewing_settings(resolution);
		}

		void update(const ComPtr<ID3D12GraphicsCommandList> &command_list) {
			command_list->RSSetViewports(1, &viewport); // set the viewports
			command_list->RSSetScissorRects(1, &scissor_rect); // set the scissor rects
		}

		bool operator==(const Rasterizer &rasterizer) const noexcept {
			return (
				viewport == rasterizer.viewport &&
				//scissor_rect == rasterizer.scissor_rect &&
				desc == rasterizer.desc
			);
		}

		void set_desc(const D3D12_RASTERIZER_DESC &desc) noexcept {
			this->desc = desc;
		}

		D3D12_RASTERIZER_DESC get_desc() const noexcept {
			return desc;
		}

		D3D12_VIEWPORT viewport = { }; // area that output from rasterizer will be stretched to.
		D3D12_RECT scissor_rect = { }; // the area to draw in. pixels outside that area will not be drawn onto
	
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
			RootArgument() { }
			RootArgument(UINT parameter_index);

		protected:
			friend RootSignature;

			std::vector<D3D12_ROOT_PARAMETER> root_parameters = { };

			UINT parameter_index = 0u;
		};

		/**
		* Descriptor table to describe data to be uploaded.
		*/
		class DescriptorTable : public RootArgument {
		public:
			DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index, UINT heap_index, UINT parameter_index);
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

		private:
			friend RootSignature;

			D3D12_ROOT_DESCRIPTOR_TABLE table;

			UINT heap_index = 0u;
			
			std::vector<D3D12_DESCRIPTOR_RANGE> ranges = { }; // In practice you often only have one descriptor range per-table.
		};

		/**
		* Root constants.
		*/
		class RootConstants : public RootArgument {
		public:
			RootConstants() { }
			RootConstants(UINT parameter_index)
				: RootArgument(parameter_index) { }

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

		private:
			friend RootSignature;
			
			void* obj = nullptr;
			size_t obj_size = 0u;

			D3D12_ROOT_CONSTANTS constants = { };
		};

		/**
		* Constant buffer.
		*/
		class ConstantBuffer {
		public:
			ConstantBuffer() { }

			template <typename T>
			ConstantBuffer(T &cb, std::string name = "")
				: obj(static_cast<void*>(&cb)), obj_size(sizeof(T)),
				name(name.empty() ? typeid(T).name() : name) { }

			/*ConstantBuffer(const ConstantBuffer &cb) {
				obj = cb.obj;
				obj_size = cb.obj_size;

				for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
					gpu_addresses[i] = cb.gpu_addresses[i];
				}
				for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
					upload_heaps[i] = cb.upload_heaps[i];
				}
			}*/

			void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12DescriptorHeap> descriptor_heaps[NUMBER_OF_BUFFERS]) {
				// create a resource heap, descriptor heap, and pointer to cbv for each frame

				for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
					ID3D12Resource* upload_buffer;

					auto type = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
					auto buf = CD3DX12_RESOURCE_DESC::Buffer((obj_size + 255) & ~255);
					HPEW(device->CreateCommittedResource(
						&type, // this heap will be used to upload the constant buffer data
						D3D12_HEAP_FLAG_NONE, // no flags
						&buf, // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
						D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
						nullptr, // we do not have use an optimized clear value for constant buffers
						IID_PPV_ARGS(&upload_buffer)));

					ResourceManager::Release::resources.push_back(upload_buffer);
					upload_buffer->SetName(L"CB upload buffer");

					D3D12_CONSTANT_BUFFER_VIEW_DESC view_desc = { };
					view_desc.BufferLocation = upload_buffer->GetGPUVirtualAddress();
					view_desc.SizeInBytes = (obj_size + 255) & ~255; // CB size is required to be 256-byte aligned.

					D3D12_CPU_DESCRIPTOR_HANDLE handle = { };
					handle.ptr = descriptor_heaps[i]->GetCPUDescriptorHandleForHeapStart().ptr + heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					device->CreateConstantBufferView(&view_desc, handle);

					CD3DX12_RANGE read_range(0, 0);	// We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
					HPEW(upload_buffer->Map(0, &read_range, reinterpret_cast<void**>(&gpu_addresses[i])));

					apply(i);
				}
			}

			void apply(int frame_index) noexcept;

			bool operator==(const ConstantBuffer &cb) const noexcept;

			void* obj = nullptr;
			size_t obj_size = 0u;

			UINT* gpu_addresses[NUMBER_OF_BUFFERS] = { };

			UINT heap_index = 0u;

			mutable std::string name = "";

			bool update_signal = true;
		};

		/**
		* Shader resource view. Used for uploading texture data.
		*/
		class ShaderResourceView {
		public:
			ShaderResourceView() { }
			ShaderResourceView(const DirectX::ScratchImage &mip_chain, bool is_texture_cube = false);

			void update_descs(const DirectX::ScratchImage &mip_chain, bool is_texture_cube = false);

			void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const ComPtr<ID3D12DescriptorHeap> descriptor_heaps[NUMBER_OF_BUFFERS]);
			void compile() { compile_signal = true; }

			ComPtr<ID3D12Resource> default_buffer{nullptr};
			D3D12_RESOURCE_DESC heap_desc{};

			std::vector<D3D12_SUBRESOURCE_DATA> subresources{};

			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};

			UINT index{0u};
			UINT heap_index{0u};

			bool compile_signal{false};
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

			void apply() { cb.update_signal = true; }

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

			std::shared_ptr<T> obj = { };
			GraphicsPipeline::RootSignature::RootConstants rc;
		};

		RootSignature() { }

		void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list);

		void update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index);

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

		ComPtr<ID3D12RootSignature> signature = nullptr; // Root signature defines data shaders will access

	private:
		ComPtr<ID3D12DescriptorHeap> descriptor_heaps[NUMBER_OF_BUFFERS] = { }; // Stores the descriptor to our constant buffer
		std::vector<DescriptorTable> descriptor_tables = { };

		std::vector<ConstantBuffer*> constant_buffers = { };
		std::vector<RootConstants*> root_constants = { };
		std::vector<ShaderResourceView*> shader_resource_views = { };

		CD3DX12_ROOT_SIGNATURE_DESC signature_desc = { };

		std::vector<D3D12_ROOT_PARAMETER> compilation_params = { };
	} root_signature;
};
