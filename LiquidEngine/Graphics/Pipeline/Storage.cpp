#include "Storage.h"

ShaderStorage* ShaderStorage::shader_storage = nullptr;

ShaderStorage* ShaderStorage::get_instance() {
	if (shader_storage == nullptr){
		shader_storage = new ShaderStorage{};
	}
	return shader_storage;
}

std::optional<std::weak_ptr<Shader>> ShaderStorage::get_shader(const std::string &file) noexcept {
	for (std::shared_ptr<Shader>& shader : shaders) {
		if (*shader == file) {
			return std::weak_ptr<Shader>{shader};
		}
	}
	return std::nullopt;
}

std::weak_ptr<Shader> ShaderStorage::add_and_compile_shader(Shader::Type type, const std::string &file) {
	if (file.empty()) return std::weak_ptr<Shader>{};
	
	for (std::shared_ptr<Shader>& shader : shaders) {
		if (*shader == file) {
			return std::weak_ptr<Shader>{shader};
		}
	}

	std::shared_ptr<Shader> s{std::make_shared<Shader>(type, file)};
	s->compile();

	shaders.push_back(s);

	return std::weak_ptr<Shader>{s};
}
