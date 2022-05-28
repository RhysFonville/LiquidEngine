#include "Material.h"

Material::Material() : texture(Texture()) {
	// Create Pixel Shader
	HANDLE_POSSIBLE_EXCEPTION(D3DReadFileToBlob(string_to_wstring(pixel_shader_name).c_str(), pixel_blob.GetAddressOf()));

	// Create Vertex Shader
	HANDLE_POSSIBLE_EXCEPTION(D3DReadFileToBlob(string_to_wstring(vertex_shader_name).c_str(), vertex_blob.GetAddressOf()));
}
