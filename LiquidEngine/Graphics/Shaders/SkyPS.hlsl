SamplerState STATIC_SAMPLER : register(s0);
Texture2D ALBEDO_TEXTURE : register(t0);

struct PSSkyCB {
	int has_texture;
	float4 albedo;
};
ConstantBuffer<PSSkyCB> SKY_BUFFER : register(b2);

struct SkyboxVSOut {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD;
};

float4 main(SkyboxVSOut ps_in) : SV_TARGET {
	if (SKY_BUFFER.has_texture)
		return ALBEDO_TEXTURE.Sample(STATIC_SAMPLER, ps_in.texcoord);
	else
		return SKY_BUFFER.albedo;
}
