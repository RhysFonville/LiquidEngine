#define NFIRST_AVAILIBLE_CB_INDEX 4
#define FIRST_AVAILIBLE_CB_INDEX b4

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 world_position : POSITION;
};

typedef VS_OUTPUT PS_INPUT;

static matrix IDENTITY_MATRIX =
{
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

static matrix ZERO_MATRIX =
{
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }
};
