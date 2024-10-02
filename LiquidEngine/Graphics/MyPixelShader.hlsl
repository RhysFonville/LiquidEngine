#include "BaseLitPS.hlsli"

cbuffer TimeCB : register(b5) {
	float time;
}

float4 main(PS_INPUT ps_in) : SV_TARGET {
	return float4(material.kd.xyz * sin(time), 1.0f);
}