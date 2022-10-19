#pragma once

#include "Component.h"

class DirectionalLightComponent : public Component {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		XMFLOAT3 direction;
		float pad;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

		ConstantBufferStruct() : direction(XMFLOAT3()),
			diffuse(XMFLOAT4()), specular(XMFLOAT4()) { }

		ConstantBufferStruct(XMFLOAT3 dir, XMFLOAT4 diff, XMFLOAT4 spec)
			: direction(dir), diffuse(diff), specular(spec) { }
	};

public:
	DirectionalLightComponent();

	void operator=(const DirectionalLightComponent &light) noexcept;

	operator ConstantBufferStruct() const noexcept {
		return ConstantBufferStruct(direction / 255.0f, diffuse / 255.0f, specular / 255.0f);
	}

	FVector3 direction;
	FVector4 diffuse;
	FVector4 specular;

	static const Type component_type = Type::DirectionalLightComponent;
};
