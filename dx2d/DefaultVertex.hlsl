#include "BaseVS.hlsl"

VS_OUTPUT calculate_vs_output(float3 position, float2 texcoord, float3 normal, float3 tangent) {
	VS_OUTPUT output;

	output.world_position = mul(float4(position, 1.0f), transform).xyz;
	output.position = float4(output.world_position, 1.0f);

	if (any(WVP != IDENTITY_MATRIX)) {
		output.position = mul(output.position, WVP);
	} else {
		output.position = float4(position, 1.0f);
	}

	output.texcoord = texcoord;
	output.normal = normal;
	output.tangent = tangent;

	return output;
}

VS_OUTPUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) {
	return calculate_vs_output(position, texcoord, normal, tangent);
}