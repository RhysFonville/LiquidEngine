#pragma once

#include <vector>
#include "../Components/CameraComponent.h"
#include "../Components/DirectionalLightComponent.h"
#include "../Components/PointLightComponent.h"
#include "../Components/SpotlightComponent.h"
#include "../Components/StaticMeshComponent.h"
#include "Sky.h"
#include "../globalutil.h"

static constexpr UINT MAX_LIGHTS_PER_TYPE = 16u;

//_declspec(align(16))
//class DXLight {
//public:
//	DXLight() { }
//	DXLight(const LightComponent &light)
//		: albedo(light.albedo.to_vec_normalized()),
//		specular(light.specular.to_vec_normalized()) { }
//
//	FVector4 albedo = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
//	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
//	int null = 0;
//};

_declspec(align(16))
class DXDLight /*: DXLight*/ {
public:
	DXDLight() { }
	DXDLight(const DirectionalLightComponent &light)
		: albedo(light.albedo.to_vec_normalized()),
		specular(light.specular.to_vec_normalized()),
		direction(light.get_rotation()), null(0) { }

	FVector4 albedo = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
	int null = true;
};

_declspec(align(16))
class DXPLight /*: DXLight*/ {
public:
	DXPLight() { }
	DXPLight(const PointLightComponent &light, const FVector3 &pos)
		: albedo(light.albedo.to_vec_normalized()),
		specular(light.specular.to_vec_normalized()), range(light.range),
		attenuation(light.attenuation), position(pos), null(0) { }

	FVector4 albedo = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	float range = 100.0f;
	FVector3 attenuation = FVector3(0.2f, 0.2f, 0.2f);
	int null = false;
	FVector3 position = FVector3(0.0f, 0.0f, 0.0f);
};

_declspec(align(16))
class DXSLight /*: DXLight*/ {
public:
	DXSLight() { }
	DXSLight(const SpotlightComponent &light)
		: albedo(light.albedo.to_vec_normalized()),
		specular(light.specular.to_vec_normalized()),
		direction(light.get_rotation()), null(0) { }

	FVector4 albedo = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	FVector3 direction = FVector3(0.0f, 0.0f, 0.0f);
	int null = 0;
};

_declspec(align(16))
class DXMaterial {
public:
	DXMaterial() { }
	DXMaterial(const Material &material)
		: has_texture(material.has_texture()), has_normal_map(material.has_normal_map()),
		a(material.shininess), ks(material.specular.to_vec_normalized()),
		kd(material.albedo.to_vec_normalized()),
		ka(material.ambient.to_vec_normalized()) { }

	int has_texture = 1;
	int has_normal_map = 1;
	FVector2 pad = FVector2(0.0f, 0.0f);
	FVector4 ks = FVector4(0.3f, 0.3f, 0.3f, 0.3f); // Specular
	FVector4 kd = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // Diffuse
	FVector4 ka = FVector4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient
	float a = 0.5f; // Shininess
};

_declspec(align(16))
struct PerFrameVSCB { // b0
	XMMATRIX WVP = XMMatrixIdentity();
};

_declspec(align(16))
struct PerObjectVSCB { // b1
	XMMATRIX transform = XMMatrixIdentity();
};

_declspec(align(16))
struct PerFramePSCB { // b2
	FVector3 camera_position = FVector3(0.0f, 0.0f, 0.0f);

	UINT directional_light_count = 0;
	UINT point_light_count = 0;
	UINT spotlight_count = 0;

	DXDLight directional_lights[MAX_LIGHTS_PER_TYPE] = { };
	DXPLight point_lights[MAX_LIGHTS_PER_TYPE] = { };
	DXSLight spotlights[MAX_LIGHTS_PER_TYPE] = { };
};

__declspec(align(16))
struct PerObjectPSCB { // b3
	DXMaterial material;
};

__declspec(align(16))
struct SkyPSCB { // b4
	SkyPSCB() { }
	SkyPSCB(const Sky &sky) : has_texture(sky.has_texture()),
		albedo(sky.albedo) { }

	int has_texture = true;
	FVector3 pad = FVector3(0.0f, 0.0f, 0.0f);
	FVector4 albedo = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
};

struct RenderingStaticMesh {
	RenderingStaticMesh(StaticMeshComponent* smc) : mesh{smc} { }
	StaticMeshComponent* mesh;
	GraphicsPipeline::RootSignature::ConstantBufferContainer<PerObjectPSCB> material_cb = PerObjectPSCB();
	GraphicsPipeline::RootSignature::ConstantBufferContainer<PerFramePSCB> lights_cb = PerFramePSCB();
};

class GraphicsScene {
public:
	GraphicsScene() { }

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, GraphicsComponent)>
	void add_component(const T *component) {
		if (component->get_type() == Component::Type::CameraComponent) {
			camera = (CameraComponent*)component;
		} else if (LightComponent::is_light_component(*component)) {
			lights.push_back((LightComponent*)component);
		} else if (component->get_type() == Component::Type::StaticMeshComponent) {
			static_meshes.push_back(RenderingStaticMesh{(StaticMeshComponent*)component});
		}
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, GraphicsComponent)>
	void remove_component(const T *component) {
		if (component->get_type() == Component::Type::StaticMeshComponent) {
			//auto static_meshes_index = std::find(static_meshes.begin(), static_meshes.end(), (StaticMeshComponent*)component);
			//if (static_meshes_index == static_meshes.end()) {
			//	static_meshes.erase(static_meshes_index);
			//}
		}
		
		if (component->get_type() == Component::Type::CameraComponent) {
			if ((CameraComponent*)component == camera) {
				camera = nullptr;
			}
		}

		if (LightComponent::is_light_component(*component)) {
			auto lights_index = std::find(lights.begin(), lights.end(), (LightComponent*)component);
			if (lights_index == lights.end()) {
				lights.erase(lights_index);
			}
		}
	}

	void clean_up();

	//Sky sky;

private:
	friend Renderer;

	std::vector<RenderingStaticMesh> static_meshes = { };
	std::vector<LightComponent*> lights = { };
	CameraComponent* camera = nullptr;
};
