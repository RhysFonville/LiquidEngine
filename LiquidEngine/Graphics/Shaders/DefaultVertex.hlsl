#include "Includes/BaseVS.hlsli"

VS_OUTPUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL,
	float3 tangent : TANGENT/*, float3 instance_position : INSTANCE_POSITION,
	float3 instance_rotation : INSTANCE_ROTATION, float3 instance_size : INSTANCE_SIZE*/) {
	
	return calculate_vs_output(position, texcoord, normal, tangent/*, instance_position, instance_rotation, instance_size*/);
}