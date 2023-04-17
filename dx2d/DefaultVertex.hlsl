cbuffer SampleBuffer : register(b0) {
	float x;
};

float4 main(float3 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) : SV_POSITION {
	return float4(float3(position.x*x, position.y*x, position.z*x), 1.0f);
}
