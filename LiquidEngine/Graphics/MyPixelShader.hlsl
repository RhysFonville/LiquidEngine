#include "BaseLitPS.hlsli"

float4 main(PS_INPUT ps_in) : SV_TARGET {
	return object_texture.Sample(static_sampler_state, ps_in.texcoord);
}