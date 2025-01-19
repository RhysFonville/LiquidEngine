#pragma once

#include <wrl.h>
#include <dxcapi.h>
#include <d3d12shader.h>
#include <vector>
#include <filesystem>
#include "../Debug/Throw.h"

namespace fs = std::filesystem;
using namespace Microsoft::WRL;

inline ComPtr<IDxcUtils> utils{};

/**
 * Shader data for rendering.
 */
class Shader {
public:
	Shader() { }
	Shader(const std::string& target, const std::string& file_name, std::vector<DxcDefine> defines = {})
		: target{target}, file_name{file_name}, defines{defines} { }

	bool operator==(const std::string& file) {
		return file_name == file;
	}

	GET std::string get_target() const noexcept { return target; }
	GET std::string get_entrypoint() const noexcept { return entrypoint; }
	GET std::string get_file() const noexcept { return file_name; }
	GET std::vector<DxcDefine> get_defines() const noexcept { return defines; }
	GET ComPtr<IDxcResult>& get_compile_result() noexcept { return compile_result; }
	GET ComPtr<ID3D12ShaderReflection> & get_reflection() noexcept { return reflection; }

	GET ComPtr<IDxcBlob> get_bytecode_blob() const {
		ComPtr<IDxcBlob> blob{};
		HPEW(compile_result->GetResult(blob.GetAddressOf()));
		return blob;
	}

	GET 

	bool operator==(const Shader& shader) const noexcept {
		return (file_name == shader.file_name);
	}

private:
	std::string file_name{""};
	std::string entrypoint{"main"};
	std::string target{"vs_6_0"};
	std::vector<DxcDefine> defines{};
	ComPtr<IDxcResult> compile_result{};
	ComPtr<ID3D12ShaderReflection> reflection{};
};

/*
 * A custom include handler that compiles each file once within the folder specified by the -I flag in the DXC compiler.
 * \see Shader
 */

class ShaderCompilerIncludeHandler : public IDxcIncludeHandler {
public:
	HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override {
		ComPtr<IDxcBlobEncoding> encoding{nullptr};

		std::filesystem::path path = std::filesystem::path{wstring_to_string(pFilename)};
		if (std::ranges::find_if(included_files, [&](const auto& x) { return fs::equivalent(path, x); }) != included_files.end()) {
			// Return empty string blob if this file has been included before
			static const char null_str[] = "";
			utils->CreateBlobFromPinned(null_str, ARRAYSIZE(null_str), DXC_CP_ACP, encoding.GetAddressOf());
			*ppIncludeSource = encoding.Detach();
			return S_OK;
		}

		HRESULT hr = utils->LoadFile(pFilename, nullptr, encoding.GetAddressOf());
		if (SUCCEEDED(hr)) {
			included_files.push_back(path);
			*ppIncludeSource = encoding.Detach();
		}

		return hr;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
	ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

	std::vector<std::filesystem::path> included_files;
};

/*
 * Compiles shaders.
 * \see Shader
 */
class ShaderCompiler {
public:
	ShaderCompiler(ShaderCompiler& other) = delete;
	void operator=(const ShaderCompiler&) = delete;
	GET static ShaderCompiler* get_instance();

	ComPtr<IDxcResult> compile(const std::wstring& file, const std::wstring& entrypoint, const std::wstring& target, const std::vector<DxcDefine>& defines = {}, const std::vector<LPCWSTR>& additional_params = {}) {
		ComPtr<ShaderCompilerIncludeHandler> include_handler{new ShaderCompilerIncludeHandler{}};

		ComPtr<IDxcBlobEncoding> source{};
		HPEW(utils->LoadFile(file.c_str(), nullptr, source.GetAddressOf()));

		ComPtr<IDxcCompilerArgs> compiler_args{};

		std::wstring include{fs::current_path().wstring()};
		std::vector<LPCWSTR> arguments{
			L"-WX",
			L"-all_resources_bound",
			L"-I", include.c_str()
		};

		arguments.insert(arguments.cend(), additional_params.begin(), additional_params.end());

#ifdef _DEBUG
		arguments.push_back(L"-Zi");
		arguments.push_back(L"-Od");
#else
		arguments.push_back(L"-O3");
		arguments.push_back(L"-Qstrip_debug");
#endif

		HPEW(utils->BuildArguments(
			file.c_str(), entrypoint.c_str(), target.c_str(),
			arguments.data(), (UINT)arguments.size(),
			defines.data(), (UINT)defines.size(),
			compiler_args.GetAddressOf()
		));

		DxcBuffer source_buffer{
			.Ptr = source->GetBufferPointer(),
			.Size = source->GetBufferSize(),
			.Encoding = 0u
		};

		ComPtr<IDxcResult> result{};
		HPEW(compiler->Compile(&source_buffer, compiler_args->GetArguments(), (UINT32)arguments.size(), include_handler.Get(), IID_PPV_ARGS(result.GetAddressOf())));

		// Error Handling. Note that this will also include warnings unless disabled.
		ComPtr<IDxcBlobUtf8> errors{};
		HPEW(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), NULL));
		if (errors && errors->GetStringLength() > 0) {
			throw std::exception{(char*)errors->GetBufferPointer()};
		}

		return result;
	}

	ComPtr<ID3D12ShaderReflection> get_shader_reflection(const ComPtr<IDxcResult>& result) {
		ComPtr<IDxcBlob> reflection_blob{};
		HPEW(result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflection_blob), nullptr));

		const DxcBuffer reflection_buffer{
			.Ptr = reflection_blob->GetBufferPointer(),
			.Size = reflection_blob->GetBufferSize(),
			.Encoding = 0u,
		};

		ComPtr<ID3D12ShaderReflection> shader_reflection{};
		utils->CreateReflection(&reflection_buffer, IID_PPV_ARGS(&shader_reflection));
		D3D12_SHADER_DESC shader_desc{};
		HPEW(shader_reflection->GetDesc(&shader_desc));

		return shader_reflection;
	}

	void compile(Shader& shader) {
		shader.get_compile_result() = compile(
			string_to_wstring(shader.get_file()),
			string_to_wstring(shader.get_entrypoint()),
			string_to_wstring(shader.get_target()),
			shader.get_defines()
		);
		shader.get_reflection() = get_shader_reflection(shader.get_compile_result());
	}

private:
	ShaderCompiler();

	static ShaderCompiler* shader_compiler;

	ComPtr<IDxcCompiler3> compiler{};
};
