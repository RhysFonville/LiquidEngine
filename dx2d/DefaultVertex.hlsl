#include "BaseVS.hlsl"

//cbuffer PerFrameVSCB : register(b0) {
//	matrix WVP;
//}

cbuffer PerObjectVSCB : register(b0) {
	matrix transform;
}

VS_OUTPUT calculate_vs_output(float3 position, float2 texcoord, float3 normal, float3 tangent) {
	VS_OUTPUT output;

	output.world_position = mul(float4(position, 1.0f), transform);
	output.position = float4(output.world_position, 1.0f);
	//output.position = mul(output.position, WVP);
	output.texcoord = texcoord;
	output.normal = normal;
	output.tangent = tangent;

	//output.position = float4(position, 1.0f);

	return output;
}

VS_OUTPUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) {
	return calculate_vs_output(position, texcoord, normal, tangent);
}