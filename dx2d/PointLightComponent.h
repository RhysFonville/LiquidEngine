#pragma once

#include "Component.h"

class PointLightComponent : public Component {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		XMFLOAT3 position;
        float range;
		XMFLOAT3 attenuation; // Falloff: x - Constant, y - Linear, z - Exponential
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;

		ConstantBufferStruct()
			: position(XMFLOAT3()), range(0.0f), attenuation(XMFLOAT3()),
			ambient(XMFLOAT4()), diffuse(diffuse) { }

		ConstantBufferStruct(XMFLOAT3 pos, float range, XMFLOAT3 att,
			XMFLOAT4 amb, XMFLOAT4 diff)
			: position(pos), range(range), attenuation(att),
			ambient(amb), diffuse(diff) { }
	};

public:
	PointLightComponent();

	void operator=(const PointLightComponent &light) noexcept;

	operator ConstantBufferStruct() const noexcept {
		return ConstantBufferStruct(transform.position, range, attenuation,
			255.0f / ambient, 255.0f / diffuse);
	}

	FVector3 attenuation;
	float range;
	FVector4 ambient;
	FVector4 diffuse;

	static const Type component_type = Type::PointLightComponent;
};

