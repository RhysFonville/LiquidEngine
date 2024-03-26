#pragma once

#include <DirectXTex.h>
#include "GraphicsPipeline.h"
#include "../Components/GraphicsComponent.h"

/**
 * Texture for rendering onto objects.
 * \see GraphicsPipeline::RootSignature::ShaderResourceView
 */
class Texture : public GraphicsTracker {
public:
	Texture() { }
	Texture(const std::string &file);

	void compile();

	//void operator=(const Texture &texture) noexcept;

	bool operator==(const Texture &texture) const noexcept;

	void set_texture(const std::string &file, bool enable_mipmap = true);
	//void set_texture(const Texture &texture);

	GET std::string get_file() const noexcept { return file; }

	bool exists() const noexcept { return fs::exists(file); }

	/**
	 * Compile texture file.
	 * 
	 * \param file Location of file.
	 * \param image The image data to write to.
	 * \param mip_chain The mipmap data to write to.
	 */
	static void load_texture(const std::string &file, DirectX::ScratchImage &image, DirectX::ScratchImage &mip_chain, bool enable_mipmap = true);

	const DirectX::ScratchImage & get_image() const noexcept { return image; }
	const DirectX::ScratchImage & get_mip_chain() const noexcept { return mip_chain; }

private:
	std::string file = "";
	DirectX::ScratchImage image{};
	DirectX::ScratchImage mip_chain{};
};
