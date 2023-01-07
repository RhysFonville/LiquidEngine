float4 main(float3 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) : SV_POSITION {
	return float4(position, 1.0f);
}
