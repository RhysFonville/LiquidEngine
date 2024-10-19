#include "BaseLitPS.hlsli"

float4 main(PS_INPUT ps_in) : SV_TARGET {
	float4 kd = material.kd;
	if (material.has_texture) {
		kd = object_texture.Sample(static_sampler_state, ps_in.texcoord);
	}
	return kd;
}