#include "Includes/BaseLitPS.hlsli"

float4 main(PS_INPUT ps_in) : SV_TARGET {
	return calculate_lit(ps_in);
}