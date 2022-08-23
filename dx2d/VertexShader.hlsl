cbuffer per_object : register(b1) {
	matrix WVP;
	matrix transform;
}

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float3 world_position : POSITION;
};

VS_OUTPUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) {
	VS_OUTPUT output;

	output.world_position = mul(float4(position, 1.0f), transform);
	output.position = float4(output.world_position, 1.0f);
	output.position = mul(output.position, WVP);
	output.texcoord = texcoord;
	//output.normal = mul(float4(normal, 1.0f), transform);
	output.normal = normal;
	//output.tangent = tangent;

	return output;
}
