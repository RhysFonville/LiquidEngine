#include "BaseShader.hlsl"

cbuffer VSWVPConstants : register(b0) {
	matrix WVP;
}
cbuffer VSTransformConstants : register(b1) {
	matrix transform;
}

VS_OUTPUT calculate_vs_output(float3 position, float2 texcoord, float3 normal, float3 tangent) {
	VS_OUTPUT output;
	
	output.world_position = mul(float4(position, 1.0f), transform).xyz;
	output.position = mul(float4(output.world_position, 1.0f), WVP);
	output.normal = normal;
	
	output.texcoord = texcoord;
	output.tangent = tangent;
	
	return output;
}
