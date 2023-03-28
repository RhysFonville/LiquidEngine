#pragma once

#include <d3d12.h>
#include "d3dx12.h"
#include "MeshComponent.h"
#include "globalutil.h"

class GraphicsPipeline {
public:
	GraphicsPipeline() { }
	GraphicsPipeline(ComPtr<ID3D12Device> &device,
		const DXGI_SAMPLE_DESC &sample_description,
		const UVector2 &resolution);

	void update(ComPtr<ID3D12GraphicsCommandList> &command_list, const CD3DX12_CPU_DESCRIPTOR_HANDLE &rtv_handle);
	void run(ComPtr<ID3D12GraphicsCommandList> &command_list, const CD3DX12_CPU_DESCRIPTOR_HANDLE &rtv_handle);

	void compile(ComPtr<ID3D12Device> &device, const DXGI_SAMPLE_DESC &sample_description, const UVector2 &resolution);

	void clean_up();

	bool operator==(const GraphicsPipeline &pipeline) const noexcept;
	void operator=(const GraphicsPipeline &pipeline) noexcept;

	ComPtr<ID3D12PipelineState> pipeline_state_object = nullptr; // pso containing a pipeline state
	ComPtr<ID3D12RootSignature> root_signature = nullptr; // root signature defines data shaders will access

	static constexpr UINT NUMBER_OF_BUFFERS = 3u;
	
	static constexpr D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,								D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 }
	};

	class InputAssembler {
	public:
		InputAssembler() { }

		void add_mesh(const MeshData &mesh, ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index = -1);
		void remove_mesh(size_t index);

		void update(ComPtr<ID3D12GraphicsCommandList> &command_list) {
			command_list->IASetPrimitiveTopology(primitive_topology); // set the primitive topology
			command_list->IASetVertexBuffers(0, (UINT)vertex_buffer_views.size(), &vertex_buffer_views[0]); // set the vertex buffer (using the vertex buffer view)
		}

		const std::vector<D3D12_VERTEX_BUFFER_VIEW> & get_vertex_buffer_views() const noexcept;

		bool operator==(const InputAssembler &input_assembler) const noexcept {
			return (
				vertex_buffers == input_assembler.vertex_buffers &&
				vertex_buffer_views == input_assembler.vertex_buffer_views
			);
		}

	private:
		friend GraphicsPipeline;

		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		D3D12_PRIMITIVE_TOPOLOGY primitive_topology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		std::vector<ComPtr<ID3D12Resource>> vertex_buffers; // a default buffer in GPU memory that we will load vertex data for our triangle into
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views; // a structure containing a pointer to the vertex data in gpu memory
		// the total size of the buffer, and the size of each element (vertex)
	} input_assembler;

	class Shader {
	public:
		enum class Type {
			Vertex,
			Hull,
			Domain,
			Geometry,
			Pixel
		};

		Shader() { }
		Shader(Type type) { }
		Shader(Type type, const std::string &file_name)
			: type(type), file_name(file_name) {
			set_target_from_type(type);
		}

		void operator=(const Shader &shader) {
			file_name = shader.file_name;
			defines = shader.defines;
			entrypoint = shader.entrypoint;
			entrypoint = shader.entrypoint;
			target = shader.target;
			shader_compile_options = shader.shader_compile_options;
			effect_compile_options = shader.effect_compile_options;
			bytecode = shader.bytecode;
			blob = shader.blob;
			error_buffer = shader.error_buffer;
			type = shader.type;
		}

		void compile() {
			if (!file_name.empty()) {
				// compile vertex shader
				HPEW(D3DCompileFromFile(
					string_to_wstring(file_name).c_str(),
					&defines,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					entrypoint.c_str(),
					target.c_str(),
					shader_compile_options,
					effect_compile_options,
					&blob,
					&error_buffer
				), (error_buffer == nullptr ? "" : (char*)error_buffer->GetBufferPointer()));
			
				bytecode.BytecodeLength = blob->GetBufferSize();
				bytecode.pShaderBytecode = blob->GetBufferPointer();
			}
		}

		GET Type get_type() const noexcept { return type; }
		void set_type(Type type) noexcept { this->type = type; }

		bool operator==(const Shader &shader) const noexcept {
			return (file_name == shader.file_name);
		}
		
	private:
		friend GraphicsPipeline;

		std::string file_name = "";
		D3D_SHADER_MACRO defines = { };
		// Add ID3DInclude someday
		std::string entrypoint = "main";
		std::string target = "ps_5_0";
		UINT shader_compile_options = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		UINT effect_compile_options = NULL;
		D3D12_SHADER_BYTECODE bytecode = { };
		ID3DBlob *blob = nullptr; // d3d blob for holding vertex shader bytecode
		ID3DBlob* error_buffer = nullptr; // a buffer holding the error data from compilation if any

		void set_target_from_type(Type type) {
			const std::string suffix = "_5_0";
			switch (type) {
				case Type::Vertex:
					target = "vs";
					break;
				case Type::Hull:
					target = "hs";
					break;
				case Type::Domain:
					target = "ds";
					break;
				case Type::Geometry:
					target = "gs";
					break;
				case Type::Pixel:
					target = "ps";
					break;
			}
			target += suffix;
		}

		Type type = Type::Pixel;
	};
	Shader vs = Shader(Shader::Type::Vertex, "DefaultVertex.hlsl");
	Shader hs = Shader(Shader::Type::Hull);
	Shader ds = Shader(Shader::Type::Domain);
	Shader gs = Shader(Shader::Type::Geometry);
	Shader ps = Shader(Shader::Type::Pixel, "UnlitPixel.hlsl");

	class Tesselator {
	public:
		bool operator==(const Tesselator &tesselator) const noexcept { return true; }
	
	private:
		friend GraphicsPipeline;
	} tesselator;

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

		void update(ComPtr<ID3D12GraphicsCommandList> &command_list) {
			command_list->RSSetViewports(1, &viewport); // set the viewports
			command_list->RSSetScissorRects(1, &scissor_rect); // set the scissor rects
		}

		bool operator==(const Rasterizer &rasterizer) const noexcept {
			return (
				viewport == rasterizer.viewport &&
				scissor_rect == rasterizer.scissor_rect &&
				desc == rasterizer.desc
			);
		}

		void set_description(const D3D12_RASTERIZER_DESC &desc) noexcept {
			this->desc = desc;
		}

		D3D12_RASTERIZER_DESC get_description() const noexcept {
			return desc;
		}

		D3D12_VIEWPORT viewport = { }; // area that output from rasterizer will be stretched to.
		D3D12_RECT scissor_rect = { }; // the area to draw in. pixels outside that area will not be drawn onto
	
	private:
		friend GraphicsPipeline;

		D3D12_RASTERIZER_DESC desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	} rasterizer;

	class OutputMerger {
	public:
		OutputMerger() { }
		OutputMerger(ComPtr<ID3D12Device> &device, const UVector2 &resolution) {
			create_depth_stencil(resolution, device);
		}

		void compile(const UVector2 &resolution, ComPtr<ID3D12Device> &device) {
			create_depth_stencil(resolution, device);
		}

		void update(ComPtr<ID3D12GraphicsCommandList> &command_list, const CD3DX12_CPU_DESCRIPTOR_HANDLE &rtv_handle) {
			command_list->ClearDepthStencilView(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			// get a handle to the depth/stencil buffer
			CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

			// set the render target for the output merger stage (the output of the pipeline)
			command_list->OMSetRenderTargets(1, &rtv_handle, false, &dsv_handle);
		}

		void create_depth_stencil(const UVector2 &resolution, ComPtr<ID3D12Device> &device);

		bool operator==(const OutputMerger &output_merger) const noexcept {
			return (
				depth_stencil_buffer == output_merger.depth_stencil_buffer &&
				depth_stencil_descriptor_heap == output_merger.depth_stencil_descriptor_heap
			);
		}

		ComPtr<ID3D12Resource> depth_stencil_buffer = nullptr; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
		ComPtr<ID3D12DescriptorHeap> depth_stencil_descriptor_heap = nullptr; // This is a heap for our depth/stencil buffer descriptor

	private:
		friend GraphicsPipeline;

		D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	} output_merger;

	class StreamOutput {
	public:
		StreamOutput() : desc({}) { }
		StreamOutput(const D3D12_STREAM_OUTPUT_DESC &desc)
			: desc(desc) { }

		bool operator==(const StreamOutput &stream_output) const noexcept {
			return (desc == stream_output.desc);
		}

		D3D12_STREAM_OUTPUT_DESC get_description() const noexcept {
			return desc;
		}

	private:
		friend GraphicsPipeline;

		D3D12_STREAM_OUTPUT_DESC desc = {};
	} stream_output;
};
