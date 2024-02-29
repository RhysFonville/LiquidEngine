#include "BaseShader.hlsl"

#define MAX_LIGHTS_PER_TYPE 16

struct DirectionalLight {
	float4 diffuse;
	float4 specular;
	float3 direction;
	int null;
};

struct PointLight {
	float4 diffuse;
	float4 specular;
	float range;
	float3 attenuation;
	int null;
	float3 position;
};

struct Spotlight {
	float4 diffuse;
	float4 specular;
	float3 direction;
	int null;
};

struct Material {
	int has_texture;
	int has_normal_map;
	int has_environment_texture;
	float4 ks;
	float4 kd;
	float4 ka;
	float a;
};

cbuffer PSLightsCB : register(b2) {
	uint directional_light_count;
	uint point_light_count;
	uint spotlight_count;
	
	DirectionalLight directional_lights[MAX_LIGHTS_PER_TYPE];
	PointLight point_lights[MAX_LIGHTS_PER_TYPE];
	Spotlight spotlights[MAX_LIGHTS_PER_TYPE];
}

cbuffer PSCameraConstants : register(b3) {
	float3 camera_position;
}

cbuffer PerObjectPSCB : register(b4) {
	Material material;
}

Texture2D object_texture : register(t0);
Texture2D normal_map : register(t1);
Texture2D environment_texture : register(t2);
SamplerState static_sampler_state : register(s0);

static const float DISTANCE_FALLOFF_POWER = 1.0f;
static const float DISTANCE_FALLOFF_INTENSITY = 0.05f;

static const float4 ia = float4(0.0f, 0.0f, 0.0f, 1.0f);

static float4 falloff_equation(float obj_pos) {
	return pow(1.0f / distance(camera_position, obj_pos), 0.0f);
}

static float4 calculate_lit_ps_main(PS_INPUT ps_in) {
	float4 kd = material.kd;
	float4 ks = material.ks;
	float4 ka = material.ka;
	float a = material.a;

	if (material.has_texture) {
		kd = object_texture.Sample(static_sampler_state, ps_in.texcoord);
	}
	
	float4 final_color = material.ka/**ia*/;
	float4 light_final_color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 n = float3(0.0f, 0.0f, 0.0f);
	
	if (material.has_normal_map) {
		float3 normal_map_result = normal_map.Sample(static_sampler_state, ps_in.texcoord);

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
			float4 is = directional_lights[i].specular;
			float4 id = directional_lights[i].diffuse;

			float4 ks = material.ks;
			float4 kd = material.kd;
			float a = material.a;

			float3 lm = normalize(-directional_lights[i].direction);

			if (dot(lm, n) > 0.0f) {
				light_final_color += saturate(kd * dot(lm, n) * id);
			}
			
			light_final_color += ia;
			
			final_color += light_final_color;
		}
	}

	for (uint i = 0; i < point_light_count; i++) { // https://en.wikipedia.org/wiki/Phong_reflection_model
		if (!point_lights[i].null) {
			float4 is = point_lights[i].specular;
			float4 id = point_lights[i].diffuse;

			float3 lm = normalize(point_lights[i].position - ps_in.world_position);

			float d = length(lm);
			if (d > point_lights[i].range)
				continue;

			float3 rm = 2.0f * n * dot(n, lm);
			float3 v = normalize(camera_position - ps_in.world_position);

			if (dot(lm, n) > 0.0f) {
				light_final_color =
					saturate(kd * dot(lm, n) * id) // Diffuse
					+ saturate(pow(max(dot(v, rm), 0.0), a) * is) // Specular
				;
			}

			light_final_color /=
				(point_lights[i].attenuation.x) +
				(point_lights[i].attenuation.y * d) +
				(point_lights[i].attenuation.z * (d * d));

			final_color += light_final_color;
		}
	}

	//final_color = float4(pow(final_color, float4(0.45f, 0.45f, 0.45f, 1.0f)));
	
	float4 distance_falloff = saturate(
		1 /
		pow(
			distance(ps_in.world_position, camera_position),
			DISTANCE_FALLOFF_POWER
		) / DISTANCE_FALLOFF_INTENSITY
	);
	distance_falloff.a = 1.0f;

	final_color *= distance_falloff;

	return final_color;
}
