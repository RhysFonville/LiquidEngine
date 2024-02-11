#include "BaseVS.hlsl"

struct SkyboxVSOut {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD;
};

SkyboxVSOut main(float3 pos : POSITION, float2 texcoord : TEXCOORD) {
	SkyboxVSOut vs_out;
	
	vs_out.position = mul(mul(float4(pos, 1.0f), transform), WVP);
	vs_out.texcoord = texcoord;

	return vs_out;
}
