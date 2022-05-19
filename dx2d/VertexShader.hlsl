cbuffer CBuffer {
	matrix transform;
};

struct VS_OUT {
	float4 color : Color;
	float4 position : SV_Position;
};

VS_OUT main(float3 pos : Position, float4 color : Color) {
	VS_OUT ret;

	ret.color = color;
	ret.position = mul(float4(pos, 1.0f), transform);
	return ret;
}
