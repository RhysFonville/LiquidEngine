#pragma once

#include <vector>
#include "../Components/CameraComponent.h"
#include "../Components/DirectionalLightComponent.h"
#include "../Components/PointLightComponent.h"
#include "../Components/SpotlightComponent.h"
#include "../Components/StaticMeshComponent.h"
#include "../Components/SkyComponent.h"
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

/**
* Each graphics component has thir graphics-side counterpart. Graphics-side component base class.
* \see GraphicsComponent
*/
template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, GraphicsComponent)>
class RenderingComponent {
public:
	RenderingComponent() { }
	RenderingComponent(T* component) : component(component) { }

	bool operator==(T component) { return (component == *(this->component)); }

	T* component = nullptr;
};

_declspec(align(16))
class RenderingDirectionalLightData/*: DXLight*/ {
public:
	RenderingDirectionalLightData() { }
	RenderingDirectionalLightData(const DirectionalLightComponent &light)
		: albedo(light.get_albedo().to_vec_normalized()),
		specular(light.get_specular().to_vec_normalized()),
		direction(light.get_rotation()), null(light.is_null()) { }

	FVector4 albedo = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
	int null = true;
};

/**
* Graphics-side directional light.
* \see DirectionalLightComponent
*/
class RenderingDirectionalLight : public RenderingComponent<DirectionalLightComponent> {
public:
	RenderingDirectionalLight(DirectionalLightComponent* light) : RenderingComponent{light}, data{*light} { }
	
	bool update() {
		if (component->has_changed()) {
			data.albedo = component->get_albedo();
			data.specular = component->get_specular();
			data.direction = component->get_direction();
			data.null = component->is_null();
			return true;
		}
		return false;
	}

	bool operator==(DirectionalLightComponent component) { return (component == *(this->component)); }

	RenderingDirectionalLightData data{};
};

_declspec(align(16))
class RenderingPointLightData/*: DXLight*/ {
public:
	RenderingPointLightData() { }
	RenderingPointLightData(const PointLightComponent &light)
		: albedo(light.get_albedo().to_vec_normalized()),
		specular(light.get_specular().to_vec_normalized()), range(light.get_range()),
		attenuation(light.get_attenuation()), position(light.get_position()), null(light.is_null()) { }

	FVector4 albedo = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	float range = 100.0f;
	FVector3 attenuation = FVector3(0.2f, 0.2f, 0.2f);
	int null = false;
	FVector3 position = FVector3(0.0f, 0.0f, 0.0f);
};

/**
* Graphics-side point light.
* \see PointLightComponent
*/
class RenderingPointLight : public RenderingComponent<PointLightComponent> {
public:
	RenderingPointLight(PointLightComponent* light) : RenderingComponent{light}, data{*light} { }

	bool update() {
		if (component->has_changed()) {
			data.albedo = component->get_albedo().to_vec_normalized();
			data.specular = component->get_specular().to_vec_normalized();
			data.range = component->get_range();
			data.attenuation = component->get_attenuation();
			data.null = component->is_null();
			data.position = component->get_position();
			return true;
		}
		return false;
	}

	bool operator==(PointLightComponent component) { return (component == *(this->component)); }

	RenderingPointLightData data{};
};

_declspec(align(16))
class RenderingSpotlightData/*: DXLight*/ {
public:
	RenderingSpotlightData() { }
	RenderingSpotlightData(const SpotlightComponent &light)
		: albedo(light.get_albedo().to_vec_normalized()),
		specular(light.get_specular().to_vec_normalized()),
		direction(light.get_rotation()), null(light.is_null()) { }

	FVector4 albedo = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FVector4 specular = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	FVector3 direction = FVector3(0.0f, 0.0f, 0.0f);
	int null = 0;
};

/**
* Graphics-side spotlight.
* \see SpotlightComponent
*/
class RenderingSpotlight : public RenderingComponent<SpotlightComponent> {
public:
	RenderingSpotlight(SpotlightComponent* light) : RenderingComponent{light}, data{*light} { }

	bool update() {
		if (component->has_changed()) {
			data.albedo = component->get_albedo();
			data.specular = component->get_specular();
			data.direction = component->get_direction();
			return true;
		}
		return false;
	}

	bool operator==(SpotlightComponent component) { return (component == *(this->component)); }

	RenderingSpotlightData data{};
};

/**
* Graphics-side material.
* \see Material
*/
_declspec(align(16))
class RenderingMaterial {
public:
	RenderingMaterial() { }
	RenderingMaterial(const Material &material)
		: has_texture(material.has_texture()), has_normal_map(material.has_normal_map()),
		a(material.get_shininess()), ks(material.get_specular().to_vec_normalized()),
		kd(material.get_albedo().to_vec_normalized()),
		ka(material.get_ambient().to_vec_normalized()) { }

	int has_texture = 1;
	int has_normal_map = 1;
	FVector2 pad = FVector2(0.0f, 0.0f);
	FVector4 ks = FVector4(0.3f, 0.3f, 0.3f, 0.3f); // Specular
	FVector4 kd = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // Diffuse
	FVector4 ka = FVector4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient
	float a = 0.5f; // Shininess
};

_declspec(align(16))
struct VSWVPConstants { // b0
	XMMATRIX WVP = XMMatrixIdentity();
};

_declspec(align(16))
struct VSTransformConstants { // b1
	XMMATRIX transform = XMMatrixIdentity();
};

_declspec(align(16))
struct PSLightsCB { // b2
	UINT directional_light_count = 0;
	UINT point_light_count = 0;
	UINT spotlight_count = 0;

	RenderingDirectionalLightData directional_lights[MAX_LIGHTS_PER_TYPE] = { };
	RenderingPointLightData point_lights[MAX_LIGHTS_PER_TYPE] = { };
	RenderingSpotlightData spotlights[MAX_LIGHTS_PER_TYPE] = { };
};

__declspec(align(16))
struct PSCameraConstants { // b3
	FVector3 camera_position = FVector3(0.0f, 0.0f, 0.0f);
	float pad = 0.0f;
};

__declspec(align(16))
struct PSMaterialCB { // b4
	RenderingMaterial material;
};

__declspec(align(16))
struct PSSkyCB { // b2
	PSSkyCB() { }
	PSSkyCB(const SkyComponent &sky) : has_texture(sky.has_texture()),
		albedo(sky.get_albedo().to_vec_normalized()) { }

	int has_texture = false;
	FVector3 pad = FVector3(0.0f, 0.0f, 0.0f);
	FVector4 albedo = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
};

/**
* Graphics-side static mesh.
* \see StaticMeshComponent
*/
class RenderingStaticMesh : public RenderingComponent<StaticMeshComponent> {
public:
	RenderingStaticMesh() { }
	RenderingStaticMesh(StaticMeshComponent* smc) : RenderingComponent{smc} { }

	bool update() {
		if (component->has_changed()) {
			material_data.obj->material = RenderingMaterial{component->get_material()};
			material_data.apply();
			transform_data.obj->transform = component->get_transform();
			return true;
		}
		return false;
	}

	void update_lights(const std::vector<RenderingDirectionalLight> &dl,
		const std::vector<RenderingPointLight> &pl,
		const std::vector<RenderingSpotlight> &sl) {

		lights_data.obj->directional_light_count = (UINT)dl.size();
		lights_data.obj->point_light_count = (UINT)pl.size();
		lights_data.obj->spotlight_count = (UINT)sl.size();

		for (int i = 0; i < dl.size(); i++) {
			lights_data.obj->directional_lights[i] = dl[i].data;
		}
		for (int i = 0; i < pl.size(); i++) {
			lights_data.obj->point_lights[i] = pl[i].data;
		}
		for (int i = 0; i < sl.size(); i++) {
			lights_data.obj->spotlights[i] = sl[i].data;
		}

		lights_data.apply();
	}

	bool operator==(StaticMeshComponent component) { return (component == *(this->component)); }

	GraphicsPipeline::RootSignature::ConstantBufferContainer<PSMaterialCB> material_data = PSMaterialCB{};
	GraphicsPipeline::RootSignature::ConstantBufferContainer<PSLightsCB> lights_data = PSLightsCB{};
	GraphicsPipeline::RootSignature::RootConstantsContainer<VSTransformConstants> transform_data = VSTransformConstants{};
};

/**
 * Graphics-side sky.
 * \see SkyComponent
 */
class RenderingSky : public RenderingComponent<SkyComponent> {
public:
	RenderingSky() { }
	RenderingSky(SkyComponent* sky) : RenderingComponent{sky} { }

	bool update(FVector3 camera_pos) {
		transform_data.obj->transform = Transform{camera_pos};
		if (component->has_changed()) {
			data.obj->albedo = component->get_albedo();
			data.obj->has_texture = component->has_texture();
			data.apply();
			return true;
		}
		return false;
	}

	bool operator==(SkyComponent component) { return (component == *(this->component)); }

	GraphicsPipeline::RootSignature::ConstantBufferContainer<PSSkyCB> data = PSSkyCB{};
	GraphicsPipeline::RootSignature::RootConstantsContainer<VSTransformConstants> transform_data = VSTransformConstants{}; // TODO: CHANGE TO CAMERA POSITION ROOT CONSTANTS
};

/**
 * Graphics-side camera.
 * \see CameraComponent
 */
class RenderingCamera : public RenderingComponent<CameraComponent> {
public:
	RenderingCamera() { }
	RenderingCamera(CameraComponent* camera) : RenderingComponent{camera} { }

	bool update(UVector2 resolution) {
		if (component->has_changed()) {
			component->update(UVector2_to_FVector2(resolution));
			wvp_data.obj->WVP = XMMatrixTranspose(component->get_wvp());
			pos_data.obj->camera_position = component->get_position();
			return true;
		}
		return false;
	}

	bool operator==(CameraComponent component) { return (component == *(this->component)); }

	GraphicsPipeline::RootSignature::RootConstantsContainer<VSWVPConstants> wvp_data = VSWVPConstants{};
	GraphicsPipeline::RootSignature::RootConstantsContainer<PSCameraConstants> pos_data = PSCameraConstants{};
};

/**
* Contains all needed scene object data for rendering.
*/
class GraphicsScene {
public:
	GraphicsScene() { }

	/**
	 * Add GraphicsComponent to the graphics scene.
	 * 
	 * Converts component data into graphics-side structure.
	 * 
	 * \param component GraphicsComponent to add.
	 * \see GraphicsComponent
	 */
	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, GraphicsComponent)>
	void add_component(const T *component) {
		if (component->get_type() == Component::Type::CameraComponent) {
			camera = RenderingCamera{(CameraComponent*)component};
		} else if (component->get_type() == Component::Type::DirectionalLightComponent) {
			directional_lights.push_back(RenderingDirectionalLight{(DirectionalLightComponent*)component});
		} else if (component->get_type() == Component::Type::PointLightComponent) {
			point_lights.push_back(RenderingPointLight{(PointLightComponent*)component});
		} else if (component->get_type() == Component::Type::SpotlightComponent) {
			spotlights.push_back(RenderingSpotlight{(SpotlightComponent*)component});
		} else if (component->get_type() == Component::Type::StaticMeshComponent) {
			static_meshes.push_back(RenderingStaticMesh{(StaticMeshComponent*)component});
		} else if (component->get_type() == Component::Type::SkyComponent) {
			sky = RenderingSky{(SkyComponent*)component};
		}
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, GraphicsComponent)>
	void remove_component(const T *component) {
		if (component->get_type() == Component::Type::StaticMeshComponent) {
			//static_meshes.erase(std::find(static_meshes.begin(), static_meshes.end(), *(StaticMeshComponent*)component));
		}
		
		if (component->get_type() == Component::Type::CameraComponent) {
			if (camera == *(CameraComponent*)component) {
				camera = nullptr;
			}
		}

		if (component->get_type() == Component::Type::DirectionalLightComponent) {
			//directional_lights.erase(std::find(directional_lights.begin(), directional_lights.end(), *(DirectionalLightComponent*)component));
		}
		if (component->get_type() == Component::Type::PointLightComponent) {
			//point_lights.erase(std::find(point_lights.begin(), point_lights.end(), *(PointLightComponent*)component));
		}
		if (component->get_type() == Component::Type::SpotlightComponent) {
			//spotlights.erase(std::find(spotlights.begin(), spotlights.end(), *(SpotlightComponent*)component));
		}
	}

	void compile() {
		for (RenderingDirectionalLight &dl : directional_lights) {
			dl.component->has_changed(true);
		}
		for (RenderingPointLight &pl : point_lights) {
			pl.component->has_changed(true);
		}
		for (RenderingSpotlight &sl : spotlights) {
			sl.component->has_changed(true);
		}

		for (RenderingStaticMesh &mesh : static_meshes) {
			mesh.component->has_changed(true);
		}

		camera.component->has_changed(true);
		sky.component->has_changed(true);
	}

	/**
	 * Look for changes in component data and update graphics-side data accordingly.
	 * 
	 * \param resolution Render resolution. Needed for updating camera.
	 */
	void update(UVector2 resolution) {
		bool light_update = false;
		for (RenderingDirectionalLight &dl : directional_lights) {
			if (!light_update) light_update = dl.update();
		}
		for (RenderingPointLight &pl : point_lights) {
			if (!light_update) light_update = pl.update();
		}
		for (RenderingSpotlight &sl : spotlights) {
			if (!light_update) light_update = sl.update();
		}

		for (RenderingStaticMesh &mesh : static_meshes) {
			mesh.update();
			if (light_update) mesh.update_lights(directional_lights, point_lights, spotlights);
		}

		camera.update(resolution);
		sky.update(camera.pos_data.obj->camera_position);

		for (RenderingDirectionalLight &dl : directional_lights) {
			dl.component->has_changed(false);
		}
		for (RenderingPointLight &pl : point_lights) {
			pl.component->has_changed(false);
		}
		for (RenderingSpotlight &sl : spotlights) {
			sl.component->has_changed(false);
		}

		for (RenderingStaticMesh &mesh : static_meshes) {
			mesh.component->has_changed(false);
		}

		camera.component->has_changed(false);
		sky.component->has_changed(false);
	}

	void clean_up();

private:
	friend Renderer;

	std::vector<RenderingStaticMesh> static_meshes = { };
	std::vector<RenderingDirectionalLight> directional_lights = { };
	std::vector<RenderingPointLight> point_lights = { };
	std::vector<RenderingSpotlight> spotlights = { };
	RenderingCamera camera{};
	RenderingSky sky{};
};
