#include "BaseLitPS.hlsl"

float4 main(VS_OUTPUT vs_out) : SV_TARGET {
	return calculate_lit_ps_output(vs_out);
}