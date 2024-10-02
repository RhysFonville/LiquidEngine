#include "BaseLitPS.hlsli"

float4 main(PS_INPUT ps_in) : SV_TARGET {
	float4 kd = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (material.has_texture) {
		kd = object_texture.Sample(static_sampler_state, ps_in.texcoord);
	}
	return kd;
}