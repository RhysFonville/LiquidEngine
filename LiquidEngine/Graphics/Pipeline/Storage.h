#pragma once

#include <wrl.h>
#include <dxcapi.h>
#include <d3d12shader.h>
#include <vector>
#include <filesystem>
#include "../Debug/Throw.h"

namespace fs = std::filesystem;
using namespace Microsoft::WRL;

#define HPEW_ERR_BLOB_PARAM(buf) ((buf == nullptr ? "" : (char*)buf->GetBufferPointer()))

inline ComPtr<IDxcUtils> utils{};

class CustomIncludeHandler : public IDxcIncludeHandler {
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

/**
 * Shader data for rendering.
 */
class Shader {
public:
	Shader() { }
	Shader(const std::string& target, const std::string &file_name)
		: target{target}, file_name{file_name} { }

	bool operator==(const std::string &file) { 
		return file_name == file;
	}

	void compile(const ComPtr<IDxcCompiler3>& compiler) {
		ComPtr<IDxcBlobEncoding> source{};
		
		std::wstring wsource{string_to_wstring(file_name)};
		HPEW(utils->LoadFile(wsource.c_str(), nullptr, source.GetAddressOf()));

		fs::path cd{fs::current_path()};
		fs::current_path(fs::path{file_name}.parent_path());

		std::wstring wentry{string_to_wstring(entrypoint)};
		std::wstring wtarget{string_to_wstring(target)};
		std::wstring winclude{string_to_wstring(fs::current_path().root_path().string())};

		std::vector<LPCWSTR> arguments{
			L"-E", wentry.c_str(),
			L"-T", wtarget.c_str(),
			L"-Qstrip_debug",
			L"-Qstrip_reflect",
			DXC_ARG_WARNINGS_ARE_ERRORS,
			DXC_ARG_ALL_RESOURCES_BOUND,
			L"-I", winclude.c_str()
		};

#ifdef _DEBUG
		arguments.push_back(DXC_ARG_DEBUG);
		arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
#else
		arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

		for (const std::string& define : defines) {
			arguments.push_back(L"-D");
			arguments.push_back(string_to_wstring(define).c_str());
		}

		ComPtr<CustomIncludeHandler> include_handler{new CustomIncludeHandler{}};
		/*for (const auto& entry : fs::directory_iterator("Includes")) {
			include_handler->included_files.push_back(entry.path());
		}*/

		DxcBuffer source_buffer{
			.Ptr = source->GetBufferPointer(),
			.Size = source->GetBufferSize(),
			.Encoding = 0u
		};

		HPEW(compiler->Compile(&source_buffer, arguments.data(), (UINT32)arguments.size(), include_handler.Get(), IID_PPV_ARGS(compile_result.GetAddressOf())));

		// Error Handling. Note that this will also include warnings unless disabled.
		ComPtr<IDxcBlobUtf8> errors{};
		HPEW(compile_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), NULL));
		if (errors && errors->GetStringLength() > 0) {
			std::cout << (char*)errors->GetBufferPointer();
			throw std::exception{(char*)errors->GetBufferPointer()};
		}

		fs::current_path(cd);
	}

	GET std::string get_target() const noexcept { return target; }
	GET std::string get_file() const noexcept { return file_name; }
	GET std::vector<std::string> get_defines() const noexcept { return defines; }
	GET ComPtr<IDxcResult> get_compile_result() const noexcept { return compile_result; }
	GET ComPtr<IDxcBlob> get_bytecode_blob() const {
		ComPtr<IDxcBlob> blob{};
		HPEW(compile_result->GetResult(blob.GetAddressOf()));
		return blob;
	}

	bool operator==(const Shader &shader) const noexcept {
		return (file_name == shader.file_name);
	}

private:
	std::string file_name{""};
	std::string entrypoint{"main"};
	std::string target{"vs_6_0"};
	std::vector<std::string> defines{};
	ComPtr<IDxcResult> compile_result{};
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

	std::weak_ptr<Shader> add_and_compile_shader(const std::string& target, const std::string &file);

private:
	ShaderStorage();

	static ShaderStorage* shader_storage;
	std::vector<std::shared_ptr<Shader>> shaders{};

	ComPtr<IDxcCompiler3> compiler{};
	ComPtr<IDxcIncludeHandler> include_handler{};
};

static ShaderStorage* shader_storage = ShaderStorage::get_instance();
