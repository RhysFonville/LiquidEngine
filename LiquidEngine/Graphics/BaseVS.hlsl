#include "BaseShader.hlsl"
#include "WVPVS.hlsl"
#include "BaseObjectVS.hlsl"

float4 calculate_vertex_screen_position(float3 position) {
	float4 out_pos;
	out_pos = mul(float4(position, 1.0f), transform);
	
	if (any(WVP != IDENTITY_MATRIX)) {
		out_pos = mul(out_pos, WVP);
	}
	
	return out_pos;
}

VS_OUTPUT calculate_vs_output(float3 position, float2 texcoord, float3 normal, float3 tangent) {
	VS_OUTPUT output;

	output.world_position = mul(float4(position, 1.0f), transform).xyz;
	output.position = float4(output.world_position, 1.0f);

	if (any(WVP != IDENTITY_MATRIX)) {
		output.position = mul(output.position, WVP);
	}

	output.texcoord = texcoord;
	output.normal = normal;
	output.tangent = tangent;

	return output;
}
