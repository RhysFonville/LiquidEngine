//#include "BaseLitPS.hlsl"

float4 main() : SV_TARGET {
	//return calculate_lit_ps_output(vs_out);
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}