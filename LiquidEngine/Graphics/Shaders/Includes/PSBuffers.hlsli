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
	int has_specular_map;
	float4 ks;
	float4 kd;
	float4 ka;
	float a;
};

#define MAX_LIGHTS_PER_TYPE 255

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

Texture2D albedo_texture : register(t0);
Texture2D normal_map : register(t1);
Texture2D specular_map : register(t2);
SamplerState static_sampler_state : register(s0);