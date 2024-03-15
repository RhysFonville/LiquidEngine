struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 world_position : POSITION;
};

typedef VS_OUTPUT PS_INPUT;

static const matrix IDENTITY_MATRIX =
{
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

static const matrix ZERO_MATRIX =
{
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }
};

static float4 invert(float4 color) {
	return float4(1-color.r, 1-color.g, 1-color.b, color.a);
}
