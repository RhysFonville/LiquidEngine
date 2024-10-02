#include "BaseLitPS.hlsli"

float4 main(PS_INPUT ps_in) : SV_TARGET {
	return calculate_lit_ps_main(ps_in);
}