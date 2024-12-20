#pragma once

#include <d3dcompiler.h>
#include <d3d12.h>
#include <ranges>
#include <filesystem>
#include <wrl.h>
#include "../Debug/Throw.h"

namespace fs = std::filesystem;
using namespace Microsoft::WRL;

#define HPEW_ERR_BLOB_PARAM(buf) ((buf == nullptr ? "" : (char*)buf->GetBufferPointer()))

/**
 * Shader data for rendering.
 */
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

	bool operator==(const std::string &file) {
		return (file_name == file);
	}

	void compile() {
		if (!file_name.empty()) {
			if (fs::exists(file_name)) {
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
				), HPEW_ERR_BLOB_PARAM(error_buffer));

				bytecode.BytecodeLength = blob->GetBufferSize();
				bytecode.pShaderBytecode = blob->GetBufferPointer();
			} else {
				throw std::exception{"Shader file does not exist."};
			}
		}
	}

	GET Type get_type() const noexcept { return type; }
	GET std::string get_file() const noexcept { return file_name; }
	GET D3D12_SHADER_BYTECODE get_bytecode() const noexcept { return bytecode; }

	bool operator==(const Shader &shader) const noexcept {
		return (file_name == shader.file_name);
	}

private:
	std::string file_name = "";
	D3D_SHADER_MACRO defines = { };
	// Add ID3DInclude someday
	std::string entrypoint = "main";
	std::string target = "ps_5_0";
	UINT shader_compile_options = D3DCOMPILE_DEBUG;
	UINT effect_compile_options = NULL;
	D3D12_SHADER_BYTECODE bytecode = { };
	ComPtr<ID3DBlob> blob = nullptr; // d3d blob for holding vertex shader bytecode
	ComPtr<ID3DBlob> error_buffer = nullptr; // a buffer holding the error data from compilation if any

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

/**
 * Stores all compiled shaders from graphics pipeline compilation. Prevents shaders from being compiled multiple times for every object that needs it.
 * \see Shader
 */
class ShaderStorage {
public:
	ShaderStorage(ShaderStorage &other) = delete;
	void operator=(const ShaderStorage &) = delete;
	GET static ShaderStorage *get_instance();

	GET std::optional<std::weak_ptr<Shader>> get_shader(const std::string &file) noexcept;

	std::weak_ptr<Shader> add_and_compile_shader(Shader::Type type, const std::string &file);

private:
	ShaderStorage() { }

	static ShaderStorage* shader_storage;
	std::vector<std::shared_ptr<Shader>> shaders;
};

static ShaderStorage* shader_storage = ShaderStorage::get_instance();
