#pragma once

#include <vector>
#include "../Components/CameraComponent.h"
#include "../Components/DirectionalLightComponent.h"
#include "../Components/PointLightComponent.h"
#include "../Components/SpotlightComponent.h"
#include "../Components/StaticMeshComponent.h"
#include "../globalutil.h"

static constexpr UINT MAX_LIGHTS_PER_TYPE = 16u;

_declspec(align(16))
struct DXDLData {
	DXDLData() { }
	DXDLData(const DirectionalLightComponent::DLData &data)
		: direction(data.direction),
		diffuse(ctofvec(data.diffuse)/255.0f),
		specular(ctofvec(data.specular)/255.0f) { }

	FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
	float pad = 0.0f;
	FVector4 diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	int null = false;

	FVector3 pad1 = FVector3(0.0f, 0.0f, 0.0f);
};

_declspec(align(16))
struct DXPLData {
	DXPLData() { }
	DXPLData(const PointLightComponent::PLData &data, const FVector3 &pos)
		: range(data.range), attenuation(data.attenuation),
		diffuse(ctofvec(data.diffuse)/255.0f), 
		specular(ctofvec(data.specular)/255.0f),
		position(pos) { }


	float range = 100.0f;
	FVector3 attenuation = FVector3(0.2f, 0.2f, 0.2f);
	FVector4 diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	int null = false;

	FVector3 position = FVector3(0.0f, 0.0f, 0.0f);
};

_declspec(align(16))
struct DXSLData {
	DXSLData() { }
	DXSLData(const SpotlightComponent::SLData &data)
		: direction(data.direction),
		diffuse(ctofvec(data.diffuse)/255.0f),
		specular(ctofvec(data.specular)/255.0f) { }


	FVector3 direction = FVector3(0.0f, 0.0f, 0.0f);
	FVector4 diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	int null = 0;

	FVector3 pad = FVector3(0.0f, 0.0f, 0.0f);;
};

_declspec(align(16))
struct DXMatData {
	DXMatData() { }
	DXMatData(const Material::MaterialData &data)
		: a(data.a), ks(color_to_fvector(data.ks)/255.0f),
		kd(color_to_fvector(data.kd)/255.0f),
		ka(color_to_fvector(data.ka)/255.0f) { }

	FVector4 ks = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // Specular
	FVector4 kd = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // Diffuse
	FVector4 ka = FVector4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient
	float a = 0.5f; // Shininess
};

_declspec(align(256))
struct PerFrameVSCB { // b0
	XMMATRIX WVP;
};

//_declspec(align(256))
struct PerObjectVSCB { // b1
	XMMATRIX transform;
};

//_declspec(align(256))
struct PerFramePSCB { // b2
	FVector3 camera_position;

	UINT directional_light_count = 0;
	UINT point_light_count = 0;
	UINT spotlight_count = 0;

	DXDLData directional_lights[MAX_LIGHTS_PER_TYPE] = { };
	DXPLData point_lights[MAX_LIGHTS_PER_TYPE] = { };
	DXSLData spotlights[MAX_LIGHTS_PER_TYPE] = { };
};

//__declspec(align(256))
struct PerObjectPSCB { // b3
	DXMatData material;
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
			static_meshes.push_back((StaticMeshComponent*)component);
		}
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, GraphicsComponent)>
	void remove_component(const T *component) {
		if (component->get_type() == Component::Type::StaticMeshComponent) {
			auto static_meshes_index = std::find(static_meshes.begin(), static_meshes.end(), (StaticMeshComponent*)component);
			if (static_meshes_index == static_meshes.end()) {
				static_meshes.erase(static_meshes_index);
			}
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
private:
	friend class Renderer;

	std::vector<StaticMeshComponent*> static_meshes = { };
	CameraComponent* camera = nullptr;
	std::vector<LightComponent*> lights = { };
};
