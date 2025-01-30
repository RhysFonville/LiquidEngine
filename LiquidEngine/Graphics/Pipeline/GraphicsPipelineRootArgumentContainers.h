#pragma once

#include <memory>
#include "GraphicsPipelineRootArguments.h"
#include "../Texture.h"

/**
* Stores a structure and the constant buffer. The structure is the data to be written to the constant buffer. Not required, but useful.
* \see ConstantBuffer
*/
template <typename T>
class ConstantBufferContainer {
public:
	ConstantBufferContainer() { }

	ConstantBufferContainer(const T &obj)
		: obj(std::make_shared<T>(obj)) { }

	void clean_up() {
		obj = nullptr;
		cb.reset();
	}

	void update() {
		if (auto cb_sp{cb.lock()}; cb_sp)
			cb_sp->update();
	}

	const std::shared_ptr<T>& get_obj() const noexcept {
		return obj;
	}

	void set_obj_ptr(const T&& obj) {
		this->obj = std::make_shared<T>(obj);
		if (auto sp{cb.lock()}) sp->set_obj(this->obj.get());
	}

	void set_obj_ptr(T& obj) {
		this->obj = std::make_shared<T>(obj);
		if (auto sp{cb.lock()}) sp->set_obj(this->obj.get());
	}

	/*const std::weak_ptr<ConstantBuffer>& get_cb() {
		return cb;
	}*/

	void set_cb(const std::weak_ptr<ConstantBuffer>& cb) {
		this->cb = cb;
		if (auto sp{cb.lock()}) sp->set_obj(obj.get());
	}

	bool operator==(const ConstantBufferContainer &cb) const noexcept {
		return (obj == cb.obj);
	}

private:
	std::shared_ptr<T> obj{};
	std::weak_ptr<ConstantBuffer> cb{};
};

/**
* Stores a structure and the root constants. The structure is the data to be written to the root constants. Not required, but useful.
*/
template <typename T>
class RootConstantsContainer {
public:
	RootConstantsContainer() { }

	RootConstantsContainer(const T& obj)
		: obj(std::make_shared<T>(obj)) { }

	void clean_up() {
		obj = nullptr;
		rc.reset();
	}

	const std::shared_ptr<T>& get_obj() const noexcept {
		return obj;
	}

	void set_obj_ptr(const T&& obj) {
		this->obj = std::make_shared<T>(obj);
		if (auto sp{rc.lock()}) sp->set_obj(this->obj.get());
	}

	void set_obj_ptr(T& obj) {
		this->obj = std::make_shared<T>(obj);
		if (auto sp{rc.lock()}) sp->set_obj(this->obj.get());
	}

	void set_obj_ptr(const std::shared_ptr<T>& obj) {
		this->obj = obj;
		if (auto sp{rc.lock()}) sp->set_obj(this->obj.get());
	}

	/*const std::weak_ptr<RootConstants>& get_rc() {
		return rc;
	}*/

	void set_rc(const std::weak_ptr<RootConstants>& rc) {
		this->rc = rc;
		if (auto sp{rc.lock()}) sp->set_obj(obj.get());
	}

	bool operator==(const RootConstantsContainer& rc) const noexcept {
		return (obj == rc.obj);
	}

private:
	std::shared_ptr<T> obj{};
	std::weak_ptr<RootConstants> rc{};
};

/**
* Stores a texture and the shader resource view. The texture is the data to be written to the shader resource view. Not required, but useful.
* \see ConstantBuffer
*/
class ShaderResourceViewContainer {
public:
	ShaderResourceViewContainer() { }

	ShaderResourceViewContainer(const Texture& texture)
		: texture(std::make_shared<Texture>(texture)) { }

	void clean_up() {
		texture = nullptr;
		srv.reset();
	}

	void update() {
		if (auto srv_sp{srv.lock()}; srv_sp) {
			srv_sp->update_descs(texture->get_mip_chain());
		}
	}

	void compile() {
		if (auto srv_sp{srv.lock()}; srv_sp) {
			srv_sp->compile();
		}
	}

	const std::shared_ptr<Texture>& get_texture() const noexcept {
		return texture;
	}

	void set_texture(const Texture& texture) {
		this->texture = std::make_shared<Texture>(texture);
		if (auto sp{srv.lock()}) sp->update_descs(texture.get_mip_chain());
	}

	void set_texture(const Texture&& texture) {
		this->texture = std::make_shared<Texture>(texture);
		if (auto sp{srv.lock()}) sp->update_descs(texture.get_mip_chain());
	}

	/*const std::weak_ptr<ShaderResourceView>& get_srv() {
		return srv;
	}*/

	void set_srv(const std::weak_ptr<ShaderResourceView>& srv) {
		this->srv = srv;
		if (auto sp{srv.lock()}) sp->update_descs(texture->get_mip_chain());
	}

	bool operator==(const ShaderResourceViewContainer& cb) const noexcept {
		return (texture == cb.texture);
	}

private:
	std::shared_ptr<Texture> texture{};
	std::weak_ptr<ShaderResourceView> srv{};
};
