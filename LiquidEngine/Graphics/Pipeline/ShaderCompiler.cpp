#include "ShaderCompiler.h"

ShaderCompiler* ShaderCompiler::shader_compiler = nullptr;

ShaderCompiler::ShaderCompiler() {
	HPEW(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));
	HPEW(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));
}

ShaderCompiler* ShaderCompiler::get_instance() {
	if (shader_compiler == nullptr) {
		shader_compiler = new ShaderCompiler{};
	}
	return shader_compiler;
}
