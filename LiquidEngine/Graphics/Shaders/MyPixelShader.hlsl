#include "Includes/BaseLitPS.hlsli"

struct Wave {
	float amplitude;
	float frequency;
	float phase;
	float time;
};
ConstantBuffer<Wave> WAVE_CONSTANTS : register(b5);

float4 main(PS_INPUT ps_in) : SV_TARGET {
    return calculate_lit(ps_in);
}
