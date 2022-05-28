cbuffer position_cbuffer {
	matrix transform;
	float4x4 WVP;
};

/*struct VS_OUT {
	float4 color : Color;
	float4 position : SV_Position;
};

VS_OUT main(float3 pos : Position, float4 color : Color) {
	VS_OUT ret;

	ret.color = color;
	ret.position = mul(float4(pos, 1.0f), transform);
	return ret;
}*/

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

VS_OUTPUT main(float3 position : Position, float2 texcoord : TEXCOORD) {
	VS_OUTPUT output;

	output.position = mul(float4(position, 1.0f), transform);
	output.texcoord = texcoord;

	return output;
}
