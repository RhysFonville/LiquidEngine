#define MAX_LIGHTS_PER_TYPE 16

Texture2D object_texture : register(t0);
Texture2D normal_map : register(t1);
SamplerState texture_sampler_state;

const uint falloff = 2;

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float3 transform_position : POSITION; // World Position
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
};

struct DirectionalLight {
	float3 direction;
	float4 ambient;
	float4 diffuse;
};

struct PointLight {
	float3 position;
	float range;
	float3 attenuation;
	float4 ambient;
	float4 diffuse;
};

struct Spotlight {
	float3 direction;
	float4 ambient;
	float4 diffuse;
};

struct Material {
	bool has_texture;
	bool has_normal_map;
	float4 diffuse;
	float specular;
	float shininess;
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
	float4 diffuse = material.diffuse;

	if (material.has_texture)
		diffuse += object_texture.Sample(texture_sampler_state, input.texcoord);

	float3 normal = normalize(input.normal);

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

	float4 final_color;
	float4 light_final_color;

	for (uint i = 0; i < directional_light_count; i++) {
		if (!directional_light_is_zero(directional_lights[i])) {
			float3 V = normalize(camera_position - input.transform_position);
			float3 R = reflect(normalize(directional_lights[i].direction), normalize(input.normal));
			float specular = material.shininess * (directional_lights[i].diffuse * invert(pow( saturate( max(0, dot(R, V)) ), material.specular )));

			light_final_color = directional_lights[i].ambient;
			light_final_color += saturate(dot(directional_lights[i].direction, input.normal))
				* directional_lights[i].diffuse * material.diffuse;
			light_final_color += specular;

			final_color += light_final_color;
		}
	}

	for (uint i = 0; i < point_light_count; i++) {
		if (!point_light_is_zero(point_lights[i])) {
			float3 lightToPixelVec = point_lights[i].position - input.transform_position;

			float d = length(lightToPixelVec);

			float3 finalAmbient = diffuse * point_lights[i].ambient;
			if (d > point_lights[i].range)
				final_color += float4(finalAmbient, diffuse.a);

			lightToPixelVec /= d; 

			float howMuchLight = dot(lightToPixelVec, normal);

			if (howMuchLight > 0.0f) {
				light_final_color += howMuchLight * diffuse * point_lights[i].diffuse;

				light_final_color /=
					(point_lights[i].attenuation.x) +
					(point_lights[i].attenuation.y * d) +
					(point_lights[i].attenuation.z * (d*d));
			}
			light_final_color = float4(saturate(light_final_color + finalAmbient), 1.0f);
			
			final_color += light_final_color;
		}
	}

	for (unsigned int i = 0; i < spotlight_count; i++) {
		if (!spotlight_is_zero(spotlights[i])) {
			float4 light_final_color;

			light_final_color = diffuse * spotlights[i].ambient;

			light_final_color += saturate(dot(spotlights[i].direction, normal) *
				spotlights[i].diffuse * diffuse);

			final_color += light_final_color;
		}
	}

	final_color.a = diffuse.a;

	return final_color;
}
