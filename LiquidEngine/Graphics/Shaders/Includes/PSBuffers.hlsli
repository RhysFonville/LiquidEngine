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

struct PSLightsCB {
	uint directional_light_count;
	uint point_light_count;
	uint spotlight_count;
	
	DirectionalLight directional_lights[MAX_LIGHTS_PER_TYPE];
	PointLight point_lights[MAX_LIGHTS_PER_TYPE];
	Spotlight spotlights[MAX_LIGHTS_PER_TYPE];
};
ConstantBuffer<PSLightsCB> LIGHTS_BUFFER : register(b2);

struct PSCameraConstants {
	float3 camera_position;
};
ConstantBuffer<PSCameraConstants> CAMERA_CONSTANTS : register(b3);

struct PerObjectPSCB {
	Material material;
};
ConstantBuffer<PerObjectPSCB> OBJECT_BUFFER : register(b4);

Texture2D ALBEDO_TEXTURE : register(t0);
Texture2D NORMAL_MAP : register(t1);
Texture2D SPECULAR_MAP : register(t2);
SamplerState STATIC_SAMPLER : register(s0);