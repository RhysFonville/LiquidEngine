#include "BaseShader.hlsl"

SamplerState static_sampler : register(s0);
Texture2D albedo_texture : register(t0);

cbuffer skyboxPSCB : register(b2) {
	int has_texture;
	float4 albedo;
};

struct skyboxVSOut {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD;
};

float4 main(skyboxVSOut ps_in) : SV_TARGET {
	if (has_texture)
		return albedo_texture.Sample(static_sampler, ps_in.texcoord);
	else
		return albedo;
}