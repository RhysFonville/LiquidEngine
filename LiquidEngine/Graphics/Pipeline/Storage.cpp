#include "Storage.h"

ShaderStorage* ShaderStorage::shader_storage = nullptr;

ShaderStorage* ShaderStorage::get_instance() {
	if (shader_storage == nullptr){
		shader_storage = new ShaderStorage{};
	}
	return shader_storage;
}

std::optional<std::reference_wrapper<Shader>> ShaderStorage::get_shader(const std::string &file) noexcept {
	for (Shader &shader : shaders) {
		if (shader.get_file() == file) {
			return shader;
		}
	}
	return std::nullopt;
}

void ShaderStorage::add_and_compile_shader(Shader::Type type, const std::string &file) {
	for (Shader &shader : shaders) {
		if (shader == file) {
			return;
		}
	}
	shaders.push_back(Shader{type, file});
	shaders.back().compile();
}
