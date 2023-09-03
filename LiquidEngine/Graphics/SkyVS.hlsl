#include "BaseVS.hlsl"

struct skyboxVSOut {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD;
};

skyboxVSOut main(float3 pos : POSITION, float2 texcoord : TEXCOORD) {
	skyboxVSOut vs_out;
	
	vs_out.position = calculate_vertex_screen_position(pos);
	vs_out.texcoord = texcoord;

	return vs_out;
}