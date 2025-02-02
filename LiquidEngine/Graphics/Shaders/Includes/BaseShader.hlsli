struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 world_position : POSITION;
};

typedef VS_OUTPUT PS_INPUT;

static const matrix IDENTITY_MATRIX = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

static const matrix ZERO_MATRIX = {
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }
};

static float invert(float color) {
	return 1.0f - color;
}

static float2 invert(float2 color) {
	return float2(1.0f - color.r, 1.0f - color.g);
}

static float3 invert(float3 color) {
	return float3(1.0f - color.r, 1.0f - color.g, 1.0f - color.b);
}

static float4 invert(float4 color) {
	return float4(1.0f - color.r, 1.0f - color.g, 1.0f - color.b, 1.0f - color.a);
}