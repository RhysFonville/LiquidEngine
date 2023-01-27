#pragma once

#include "Component.h"

class SpotlightComponent : public Component {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		XMFLOAT3 direction;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
	};

public:
	SpotlightComponent();

	operator ConstantBufferStruct() const noexcept {
		return { direction, diffuse / 255.0f, specular };
	}

	virtual bool operator==(const SpotlightComponent &component) const noexcept;
	virtual void operator=(const SpotlightComponent &component) noexcept;

	FVector3 direction;
	FVector4 diffuse;
	FVector4 specular;

	static const Type component_type = Type::SpotlightComponent;
};

