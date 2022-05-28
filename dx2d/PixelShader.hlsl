/*float4 main(float4 color : Color) : SV_Target{
	return color;
}*/

Texture2D object_texture;
SamplerState texture_sampler_state;

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET {
	return object_texture.Sample(texture_sampler_state, input.texcoord);
}
