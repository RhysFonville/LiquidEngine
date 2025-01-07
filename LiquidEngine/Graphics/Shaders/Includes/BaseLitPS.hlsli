#include "BaseShader.hlsli"
#include "PSBuffers.hlsli"

static const float DISTANCE_FALLOFF_POWER = 1.0f;
static const float DISTANCE_FALLOFF_INTENSITY = 0.05f;

static float4 falloff_equation(float obj_pos) {
	return pow(1.0f / distance(camera_position, obj_pos), 0.0f);
}

static float4 calculate_lit(PS_INPUT ps_in) {
	float4 kd = material.kd;
	float4 ks = material.ks;
	float4 ka = material.ka;
	float a = material.a;
	
	if (material.has_texture) {
		kd *= albedo_texture.Sample(static_sampler_state, ps_in.texcoord);
	}
	if (material.has_specular_map) {
		ks *= specular_map.Sample(static_sampler_state, ps_in.texcoord);
	}
	
	float4 final_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 light_final_color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 n = float3(0.0f, 0.0f, 0.0f);
	
	if (material.has_normal_map) {
		float3 normal_map_result = normal_map.Sample(static_sampler_state, ps_in.texcoord).xyz;

		// Change normal map range from [0, 1] to [-1, 1]
		normal_map_result = (2.0f * normal_map_result) - 1.0f;

		// Make sure tangent is completely orthogonal to normal
		ps_in.tangent = normalize(ps_in.tangent - dot(ps_in.tangent, ps_in.normal) * ps_in.normal);

		// Create the biFVector3
		float3 bitangent = cross(ps_in.normal, ps_in.tangent);

		// Create the "Texture Space"
		float3x3 tex_space = float3x3(ps_in.tangent, bitangent, ps_in.normal);

		// Convert normal from normal map to texture space and store in input.normal
		n = normalize(mul(normal_map_result, tex_space));
	} else {
		n = normalize(ps_in.normal);
	}

	for (uint i = 0; i < directional_light_count; i++) {
		if (!directional_lights[i].null) {
			float3 light_dir = normalize(-directional_lights[i].direction);
			float3 view_dir = normalize(camera_position - ps_in.world_position);
			float3 reflect_dir = reflect(-light_dir, n);
			
			float diff_power = max(dot(n, light_dir), 0.0f);
			float spec_power = pow(max(dot(view_dir, reflect_dir), 0.0f), a);
			
			float4 ambient = ka;
			float4 diffuse = (directional_lights[i].diffuse * kd) * float4(diff_power.xxx, 1.0f);
			float4 specular = (directional_lights[i].specular * ks) * float4(spec_power.xxx, 1.0f);
			
			light_final_color = (ambient + diffuse + specular);
			final_color += light_final_color;
		}
	}

	for (uint i = 0; i < point_light_count; i++) {
		if (!point_lights[i].null) {
			float distance = length(point_lights[i].position - ps_in.world_position);
			if (distance <= point_lights[i].range) {
				float3 light_dir = normalize(point_lights[i].position - ps_in.world_position);
				float3 view_dir = normalize(camera_position - ps_in.world_position);
				float3 reflect_dir = reflect(-light_dir, n);
			
				float diff_power = max(dot(n, light_dir), 0.0f);
				float spec_power = pow(max(dot(view_dir, reflect_dir), 0.0f), a);
			
				float4 ambient = ka;
				float4 diffuse = (point_lights[i].diffuse * kd) * float4(diff_power.xxx, 1.0f);
				float4 specular = (point_lights[i].specular * ks) * float4(spec_power.xxx, 1.0f);
			
				float att =
				1.0 / (
						point_lights[i].attenuation.x +
						distance * point_lights[i].attenuation.y +
						(distance * distance) * point_lights[i].attenuation.z
				);
				float4 attenuation = float4(att.xxx, 1.0f);
				
				ambient *= attenuation;
				diffuse *= attenuation;
				specular *= attenuation;
			
				light_final_color = (ambient + diffuse + specular);
				final_color += light_final_color;
			}
		}
	}
	
	//final_color = float4(pow(final_color, float4(0.45f, 0.45f, 0.45f, 1.0f)));
	
	float3 distance_falloff = saturate(
		1 /
		pow(
			distance(ps_in.world_position, camera_position),
			DISTANCE_FALLOFF_POWER
		) / DISTANCE_FALLOFF_INTENSITY
	);

	final_color *= float4(distance_falloff, 1.0f);
	
	return final_color;
}
