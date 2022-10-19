#pragma once

#include "Component.h"

class PointLightComponent : public Component {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		XMFLOAT3 position;
        float range;
		XMFLOAT3 attenuation; // Falloff: x - Constant, y - Linear, z - Exponential
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

		ConstantBufferStruct()
			: position(XMFLOAT3()), range(0.0f), attenuation(XMFLOAT3()),
			diffuse(XMFLOAT4()), specular(XMFLOAT4()) { }

		ConstantBufferStruct(XMFLOAT3 pos, float range, XMFLOAT3 att,
			XMFLOAT4 diff, XMFLOAT4 spec)
			: position(pos), range(range), attenuation(att),
			diffuse(diff), specular(spec) { }
	};

public:
	PointLightComponent();

	void operator=(const PointLightComponent &light) noexcept;

	operator ConstantBufferStruct() const noexcept {
		return ConstantBufferStruct(transform.position, range, attenuation,
			diffuse / 255.0f, specular / 255.0f);
	}

	FVector3 attenuation;
	float range;
	FVector4 diffuse;
	FVector4 specular;

	static const Type component_type = Type::PointLightComponent;
};

