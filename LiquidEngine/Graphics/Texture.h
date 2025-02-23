#pragma once

#include <DirectXTex.h>
#include <filesystem>
#include "../Components/GraphicsComponent.h"
#include "../Debug/Throw.h"

namespace fs = std::filesystem;

/**
 * Texture for rendering onto objects.
 * \see ShaderResourceView
 */
class Texture : public GraphicsTracker {
public:
	Texture() { }
	Texture(const Texture& texture);
	Texture(const std::string &file);

	void compile();

	bool operator==(const Texture &texture) const noexcept;

	void set_texture(const std::string &file, bool enable_mipmap = true);
	//void set_texture(const Texture &texture);

	/**
	 * Compile texture file.
	 * 
	 * \param file Location of file.
	 * \param image The image data to write to.
	 * \param mip_chain The mipmap data to write to.
	 */
	static void load_texture(const std::string &file, DirectX::ScratchImage &image, DirectX::ScratchImage &mip_chain, bool enable_mipmap = true);

	GET std::string get_file() const noexcept { return file; }

	GET bool exists() const noexcept { return fs::exists(file); }

	GET bool is_opaque() const noexcept;

	GET const DirectX::ScratchImage & get_image() const noexcept { return image; }
	GET const DirectX::ScratchImage & get_mip_chain() const noexcept { return mip_chain; }

private:
	std::string file = "";
	DirectX::ScratchImage image{};
	DirectX::ScratchImage mip_chain{};
};
