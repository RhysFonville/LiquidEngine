#include "BaseShader.hlsl"

cbuffer mycb : register(b2) {
	float3 col;
};

float4 main() : SV_TARGET {
	return float4(col, 1.0f);
}