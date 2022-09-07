#define MAX_LIGHTS_PER_TYPE 16

Texture2D object_texture : register(t0);
Texture2D normal_map : register(t1);
SamplerState texture_sampler_state;

static const float DISTANCE_FALLOFF_POWER = 1.0f;
static const float DISTANCE_FALLOFF_INTENSITY = 0.05f;

static const float4 ia = float4(0.0f, 0.0f, 0.0f, 1.0f);

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float3 world_position : POSITION;
};

struct DirectionalLight {
	float3 direction;
	float4 diffuse;
	float4 specular;
};

struct PointLight {
	float3 position;
	float range;
	float3 attenuation;
	float4 diffuse;
	float4 specular;
};

struct Spotlight {
	float3 direction;
	float4 diffuse;
	float4 specular;
};

struct Material {
	bool has_texture;
	bool has_normal_map;
	//float4 diffuse;
	//float specular;
	//float shininess;
	float4 ks;
	float4 kd;
	float4 ka;
	float a;
};

cbuffer per_frame : register(b0) {
	float3 pad;
	DirectionalLight directional_lights[MAX_LIGHTS_PER_TYPE];
	PointLight point_lights[MAX_LIGHTS_PER_TYPE];
	Spotlight spotlights[MAX_LIGHTS_PER_TYPE];
	uint directional_light_count;
	uint point_light_count;
	uint spotlight_count;

	float3 camera_position;
};

cbuffer per_object : register(b2) {
	Material material;
};

bool directional_light_is_zero(DirectionalLight light) {
	return ((light.diffuse.r == 0 && 
		light.diffuse.g == 0 &&
		light.diffuse.b == 0 &&
		light.diffuse.a == 0) || light.diffuse.a == 0);
}

bool point_light_is_zero(PointLight light) {
	return ((light.diffuse.r == 0 &&
		light.diffuse.g == 0 &&
		light.diffuse.b == 0 &&
		light.diffuse.a == 0) || light.diffuse.a == 0 ||
		light.range <= 0.0f);
}

bool spotlight_is_zero(Spotlight light) {
	return ((light.diffuse.r == 0 &&
		light.diffuse.g == 0 &&
		light.diffuse.b == 0 &&
		light.diffuse.a == 0) || light.diffuse.a == 0);
}

float4 invert(float4 color) {
	return float4(1-color.r, 1-color.g, 1-color.b, color.a);
}

float4 falloff_equation(float obj_pos) {
	return pow(1.0f / distance(camera_position, obj_pos), 0.0f);
}

float4 main(VS_OUTPUT input) : SV_TARGET {
	float4 ks = material.ks;
	float4 kd = material.kd;
	float4 ka = material.ka;
	float a = material.a;

	if (material.has_texture)
		kd = object_texture.Sample(texture_sampler_state, input.texcoord);

	float4 final_color = material.ka*ia;
	float4 light_final_color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 normal = input.normal;

	/*if (material.has_normal_map) {
		float3 normal_map_result = normal_map.Sample(texture_sampler_state, input.texcoord);

		//Change normal map range from [0, 1] to [-1, 1]
		normal_map_result = (2.0f*normal_map_result) - 1.0f;

		//Make sure tangent is completely orthogonal to normal
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);

		//Create the biTangent
		float3 biTangent = cross(input.normal, input.tangent);

		//Create the "Texture Space"
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

		//Convert normal from normal map to texture space and store in input.normal
		normal = normalize(mul(normal_map_result, texSpace));
	}*/

	for (uint i = 0; i < directional_light_count; i++) {
		if (!directional_light_is_zero(directional_lights[i])) {
			float4 is = directional_lights[i].specular;
			float4 id = directional_lights[i].diffuse;

			float4 ks = material.ks;
			float4 kd = material.kd;
			float a = material.a;

			float3 lm = normalize(-directional_lights[i].direction);
			float3 n = normal;
			//float3 rm = reflect(lm, n);
			//float3 rm = 2*(lm*n)*n-lm;
			//float3 v = normalize(camera_position - input.world_position);

			if (dot(lm, n) > 0.0f) {
				light_final_color += saturate(kd*dot(lm, n)*id)/* +
					saturate(ks * pow(dot(rm, v), a) * is)*/;
			}

			final_color += light_final_color;

			//light_final_color += ia;
		}
	}

	for (uint i = 0; i < point_light_count; i++) {
		if (!point_light_is_zero(point_lights[i])) {
			float4 is = point_lights[i].specular;
			float4 id = point_lights[i].diffuse;

			float3 lm = normalize(point_lights[i].position - input.world_position);

			float d = length(lm);
			if (d > point_lights[i].range)
				continue;

			float3 n = normal;
			float3 rm = 2.0f * n * dot(n, lm);
			float3 v = normalize(camera_position - input.world_position);

			if (dot(lm, n) > 0.0f) {
				light_final_color += saturate(kd*dot(lm, n) * id) +
					saturate(ks * pow(dot(rm, v), a) * is);
			}

			light_final_color /=
				(point_lights[i].attenuation.x) +
				(point_lights[i].attenuation.y * d) +
				(point_lights[i].attenuation.z * (d*d));

			final_color += light_final_color;
		}
	}

	/*for (unsigned int i = 0; i < spotlight_count; i++) {
		if (!spotlight_is_zero(spotlights[i])) {
			float4 light_final_color;

			light_final_color = diffuse * spotlights[i].ambient;

			light_final_color += saturate(dot(spotlights[i].direction, normal) *
				spotlights[i].diffuse * diffuse);

			final_color += light_final_color;
		}
	}*/

	final_color *= saturate(
		1 /
		pow(
			distance(input.world_position, camera_position),
			DISTANCE_FALLOFF_POWER
		) / DISTANCE_FALLOFF_INTENSITY
	);

	final_color.a = 255.0f;

	return final_color;
}
