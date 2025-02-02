#pragma once

#include "ShaderCompiler.h"

/**
 * Stores all compiled shaders from graphics pipeline compilation. Prevents shaders from being compiled multiple times for every object that needs it.
 * \see Shader
 */
class ShaderStorage {
public:
	ShaderStorage(ShaderStorage& other) = delete;
	void operator=(const ShaderStorage&) = delete;
	GET static ShaderStorage* get_instance();

	GET std::optional<std::weak_ptr<Shader>> get_shader(const std::string &file) noexcept;

	std::weak_ptr<Shader> add_and_compile_shader(const std::string& target, const std::string &file);

private:
	ShaderStorage() { }

	static ShaderStorage* shader_storage;
	std::vector<std::shared_ptr<Shader>> shaders{};
};
