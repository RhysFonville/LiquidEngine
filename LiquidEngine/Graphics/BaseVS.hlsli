#include "BaseShader.hlsli"

cbuffer VSWVPConstants : register(b0) {
	matrix WVP;
}
cbuffer VSTransformConstants : register(b1) {
	matrix transform;
}

static matrix position_matrix(float3 position) {
	return float4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		position.x, position.y, position.z, 1
	);
}

static matrix rotation_matrix(float3 rotation) {
	float cosX = cos(rotation.x);
	float sinX = sin(rotation.x);
	float cosY = cos(rotation.y);
	float sinY = sin(rotation.y);
	float cosZ = cos(rotation.z);
	float sinZ = sin(rotation.z);

	matrix rotX = float4x4(
		1, 0, 0, 0,
		0, cosX, sinX, 0,
		0, -sinX, cosX, 0,
		0, 0, 0, 1
	);

	matrix rotY = float4x4(
		cosY, 0, -sinY, 0,
		0, 1, 0, 0,
		sinY, 0, cosY, 0,
		0, 0, 0, 1
	);

	matrix rotZ = float4x4(
		cosZ, sinZ, 0, 0,
		-sinZ, cosZ, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	return mul(rotZ, mul(rotY, rotX));
}

static matrix size_matrix(float3 size) {
	return float4x4(
		size.x, 0, 0, 0,
		0, size.y, 0, 0,
		0, 0, size.z, 0,
		0, 0, 0, 1
	);
}

static VS_OUTPUT calculate_vs_output(float3 position, float2 texcoord, float3 normal, float3 tangent,
	float3 instance_position, float3 instance_rotation, float3 instance_size) {
	VS_OUTPUT output;
	
	matrix new_transform = mul(transform, rotation_matrix(instance_position));
	new_transform = mul(new_transform, rotation_matrix(instance_rotation));
	new_transform = mul(new_transform, size_matrix(instance_size));
	
	output.world_position = mul(float4(position, 1.0f), new_transform).
	xyz;
	output.position = mul(float4(output.world_position, 1.0f), WVP);
	output.normal = mul(float4(normal, 0.0f), new_transform).xyz;
	
	output.texcoord = texcoord;
	output.tangent = mul(float4(tangent, 0.0f), new_transform).xyz;
	
	return output;
}

static VS_OUTPUT calculate_vs_output(float3 position, float2 texcoord, float3 normal, float3 tangent) {
	VS_OUTPUT output;
	
	output.world_position = mul(float4(position, 1.0f), transform).xyz;
	output.position = mul(float4(output.world_position, 1.0f), WVP);
	output.normal = mul(float4(normal, 0.0f), transform).xyz;
	
	output.texcoord = texcoord;
	output.tangent = mul(float4(tangent, 0.0f), transform).xyz;
	
	return output;
}
