#pragma once

#include "globalutil.h"

class Component {
public:
	enum class Type {
		MeshComponent,
		CameraComponent,
		DirectionalLightComponent,
		PointLightComponent,
		SpotlightComponent
	};

	Component(const Type &type, const Transform &transform = Transform());

	virtual void set_position(FPosition3 position) noexcept;
	virtual void set_rotation(FRotation3 rotation) noexcept;
	virtual void set_size(FSize3 size) noexcept;

	virtual void translate(FPosition3 position) noexcept;
	virtual void rotate(FRotation3 rotation) noexcept;
	virtual void size(FSize3 size) noexcept;

	virtual GET FPosition3 get_position() const noexcept;
	virtual GET FRotation3 get_rotation() const noexcept;
	virtual GET FSize3 get_size() const noexcept;

	virtual void set_transform(Transform transform) noexcept;
	virtual GET Transform get_transform() const noexcept;

	virtual void clean_up() { };

	virtual void compile() noexcept { };

	bool operator==(const Component &component) const noexcept;

	const Type type;

protected:
	Transform transform;
};

