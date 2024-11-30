#pragma once

#include "GraphicsPipelineRootArguments.h"

/**
* Stores a structure and the constant buffer. The structure is the data to be written to the constant buffer. Not required, but useful.
* \see ConstantBuffer
*/
template <typename T>
class ConstantBufferContainer {
public:
	ConstantBufferContainer() { }

	ConstantBufferContainer(const T &obj)
		: obj(std::make_shared<T>(obj)),
		cb{std::make_shared<ConstantBuffer>(*this->obj)} { }

	void clean_up() {
		obj = nullptr;
		cb = nullptr;
	}

	void update() { cb->update_signal = true; }

	bool operator==(const ConstantBufferContainer &cb) const noexcept {
		return (obj == cb.obj);
	}

	std::shared_ptr<T> obj{};
	std::shared_ptr<ConstantBuffer> cb{nullptr};
};

/**
* Stores a structure and the root constants. The structure is the data to be written to the root constants. Not required, but useful.
*/
template <typename T>
class RootConstantsContainer {
public:
	RootConstantsContainer() { }

	RootConstantsContainer(const T &obj)
		: obj(std::make_shared<T>(obj)),
		rc{std::make_shared<RootConstants>()} {
		rc->set_obj<T>(this->obj.get());
	}

	void clean_up() {
		obj = nullptr;
		rc = nullptr;
	}

	bool operator==(const RootConstantsContainer& rc) const noexcept {
		return (obj == rc.obj);
	}

	std::shared_ptr<T> obj{};
	std::shared_ptr<RootConstants> rc{nullptr};
};
