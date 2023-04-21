#include "BaseVS.hlsl"

VS_OUTPUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) {
	return calculate_vs_output(position, texcoord, normal, tangent);
}