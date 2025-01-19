#pragma once

#include <array>
#include "../Components/CameraComponent.h"
#include "../Components/DirectionalLightComponent.h"
#include "../Components/PointLightComponent.h"
#include "../Components/SpotlightComponent.h"
#include "../Components/StaticMeshComponent.h"
#include "../Components/SkyComponent.h"
#include "../Utility/Vectors.h"
#include "Pipeline/GraphicsPipelineRootArgumentContainers.h"

static constexpr UCHAR MAX_LIGHTS_PER_TYPE = 255u;

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
template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, GraphicsTracker)>
class RenderingComponent {
public:
	RenderingComponent() { }
	RenderingComponent(T* component) : component(component) { }

	bool operator==(T component) { return (component == *(this->component)); }

	T* component{nullptr};
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
			data.albedo = component->get_albedo().to_vec_normalized();
			data.specular = component->get_specular().to_vec_normalized();
			
			auto rot = component->get_rotation();
			data.direction = XMVector3TransformCoord(global_forward.to_xmvec(),
				XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z)
			);

			data.null = component->is_null();

			component->has_changed(false);
			return true;
		}
		return false;
	}

	void compile() {
		component->has_changed(true);
		component->needs_compile(false);
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

			component->has_changed(false);
			return true;
		}
		return false;
	}

	void compile() {
		component->has_changed(true);
		component->needs_compile(false);
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
			data.albedo = component->get_albedo().to_vec_normalized();
			data.specular = component->get_specular().to_vec_normalized();
			data.direction = component->get_rotation();

			component->has_changed(false);
			return true;
		}
		return false;
	}

	void compile() {
		component->has_changed(true);
		component->needs_compile(false);
	}

	bool operator==(SpotlightComponent component) { return (component == *(this->component)); }

	RenderingSpotlightData data{};
};

_declspec(align(16))
class RenderingMaterialData {
public:
	RenderingMaterialData() { }
	RenderingMaterialData(const Material &material)
		: has_texture{material.has_texture()}, has_normal_map{material.has_normal_map()},
		has_specular_map{material.has_specular_map()}, a(material.get_shininess()),
		ks(material.get_specular().to_vec_normalized()),
		kd(material.get_albedo().to_vec_normalized()),
		ka(material.get_ambient().to_vec_normalized()) { }

	int has_texture = 1;
	int has_normal_map = 1;
	int has_specular_map = 1;
	float pad = 0.0f;
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

	std::array<RenderingDirectionalLightData, MAX_LIGHTS_PER_TYPE> directional_lights{};
	std::array<RenderingPointLightData, MAX_LIGHTS_PER_TYPE> point_lights{};
	std::array<RenderingSpotlightData, MAX_LIGHTS_PER_TYPE> spotlights{};
};

__declspec(align(16))
struct PSCameraConstants { // b3
	FVector3 camera_position = FVector3(0.0f, 0.0f, 0.0f);
	float pad = 0.0f;
};

__declspec(align(16))
struct PSMaterialCB { // b4
	RenderingMaterialData material{};
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
			wvp_data.get_obj()->WVP = XMMatrixTranspose(component->get_wvp());
			pos_data.get_obj()->camera_position = component->get_position();
			wvp_data.update();
			pos_data.update();

			return true;
		}
		return false;
	}

	void compile() {
		component->has_changed(true);
		wvp_data = VSWVPConstants{};
		pos_data = PSCameraConstants{};

		component->needs_compile(false);
	}

	void clean_up() {
		wvp_data.clean_up();
		pos_data.clean_up();
	}

	bool operator==(CameraComponent component) { return (component == *(this->component)); }

	ConstantBufferContainer<VSWVPConstants> wvp_data{VSWVPConstants{}};
	ConstantBufferContainer<PSCameraConstants> pos_data{PSCameraConstants{}};
};

/**
* Graphics-side texture.
*/
/*class RenderingTexture : public RenderingComponent<Texture> {
public:
	RenderingTexture() { }
	RenderingTexture(Texture* texture) : RenderingComponent{texture} { }

	bool update() {
		if (component->has_changed()) {
			update_descs_and_compile_srv();

			component->has_changed(false);
			return true;
		}
		return false;
	}

	void compile(const std::string& name) {
		camera.wvp_data.rc = component->pipeline.root_signature.get_resource(name);

		component->needs_compile(false);
	}

	void update_descs_and_compile_srv() {
		if (component->exists()) {
			srv->update_descs(component->get_mip_chain());
			srv->compile();
		}
	}

	void clean_up() {
		srv = nullptr;
	}

	ShaderResourceView srv{};
};*/

/**
* Graphics-side sky.
* \see SkyComponent
*/
class RenderingSky : public RenderingComponent<SkyComponent> {
public:
	RenderingSky() { }
	RenderingSky(SkyComponent* sky) : RenderingComponent{sky} { }

	bool update(const RenderingCamera& camera) {
		if (texture.get_texture()->has_changed()) {
			texture.update();
		}

		bool change{false};
		if (camera.component->has_changed()) change = true;

		if (component->has_changed()) {
			data.set_obj_ptr(PSSkyCB{*component});
			data.update();

			change = true;
		}

		if (change) {
			transform_data.get_obj()->transform = Transform{
				camera.pos_data.get_obj()->camera_position+component->get_position(),
				component->get_rotation(),
				component->get_size()
			};

			component->has_changed(false);
		}

		return true;
	}

	void compile(RenderingCamera& camera) {
		texture.set_texture(component->get_albedo_texture());
		transform_data = VSTransformConstants{};
		data = PSSkyCB{};

		component->has_changed(true);
		component->needs_compile(false);
	}

	void set_resources(RenderingCamera& camera) {
		camera.wvp_data.set_cb(component->pipeline.root_signature.get_constant_buffer("WVP_BUFFER"));
		transform_data.set_cb(component->pipeline.root_signature.get_constant_buffer("TRANSFORM_BUFFER"));
		data.set_cb(component->pipeline.root_signature.get_constant_buffer("SKY_BUFFER"));
		texture.set_srv(component->pipeline.root_signature.get_shader_resource_view("ALBEDO_TEXTURE"));
	}

	void clean_up() {
		texture.clean_up();
		transform_data.clean_up();
		data.clean_up();
	}

	bool operator==(SkyComponent component) { return (component == *(this->component)); }

	ShaderResourceViewContainer texture{};

	ConstantBufferContainer<VSTransformConstants> transform_data{};
	ConstantBufferContainer<PSSkyCB> data{};
};

/**
* Graphics-side material.
* \see Material
*/
class RenderingMaterial : public RenderingComponent<Material> {
public:
	RenderingMaterial() : RenderingComponent{} { }
	RenderingMaterial(Material* mat) : RenderingComponent{mat} { }

	bool update() {
		bool ret{false};
		if (albedo_texture.get_texture()->has_changed()) {
			albedo_texture.update();
			ret = true;
		}
		if (normal_map.get_texture()->has_changed()) {
			normal_map.update();
			ret = true;
		}
		if (specular_map.get_texture()->has_changed()) {
			specular_map.update();
			ret = true;
		}

		if (component->has_changed() || ret) {
			material_data.get_obj()->material = RenderingMaterialData{*component};
			material_data.update();

			component->has_changed(false);
			return true;
		}
		return ret;
	}

	void compile() {
		albedo_texture.set_texture(component->get_albedo_texture());
		normal_map.set_texture(component->get_normal_map());
		specular_map.set_texture(component->get_specular_map());
		material_data = PSMaterialCB{};

		component->has_changed(true);
		component->needs_compile(false);
	}

	void set_resources() {
		material_data.set_cb(component->pipeline.root_signature.get_constant_buffer("OBJECT_BUFFER"));
		albedo_texture.set_srv(component->pipeline.root_signature.get_shader_resource_view("ALBEDO_TEXTURE"));
		normal_map.set_srv(component->pipeline.root_signature.get_shader_resource_view("NORMAL_MAP"));
		specular_map.set_srv(component->pipeline.root_signature.get_shader_resource_view("SPECULAR_MAP"));
	}

	void clean_up() {
		material_data.clean_up();
		albedo_texture.clean_up();
		normal_map.clean_up();
		specular_map.clean_up();
	}

	ShaderResourceViewContainer albedo_texture{};
	ShaderResourceViewContainer normal_map{};
	ShaderResourceViewContainer specular_map{};

	ConstantBufferContainer<PSMaterialCB> material_data{};
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
		bool ret{false};
		if (material.update()) ret = true;

		if (component->has_changed()) {
			transform_data.get_obj()->transform = component->get_transform();
			transform_data.update();
			component->has_changed(false);
			return true;
		}
		return ret;
	}

	void update_lights(const std::vector<RenderingDirectionalLight> &dl,
		const std::vector<RenderingPointLight> &pl,
		const std::vector<RenderingSpotlight> &sl) {

		lights_data.get_obj()->directional_light_count = (UINT)dl.size();
		lights_data.get_obj()->point_light_count = (UINT)pl.size();
		lights_data.get_obj()->spotlight_count = (UINT)sl.size();

		for (int i = 0; i < dl.size(); i++) {
			lights_data.get_obj()->directional_lights[i] = dl[i].data;
		}
		for (int i = 0; i < pl.size(); i++) {
			lights_data.get_obj()->point_lights[i] = pl[i].data;
		}
		for (int i = 0; i < sl.size(); i++) {
			lights_data.get_obj()->spotlights[i] = sl[i].data;
		}

		lights_data.update();

		update_lights_signal = false;
	}

	void compile(RenderingCamera &camera) {
		material = RenderingMaterial{&component->get_material()};
		lights_data = PSLightsCB{};
		transform_data = VSTransformConstants{};
		
		material.compile();

		component->has_changed(true);
		component->needs_compile(false);
	}

	void set_resources(RenderingCamera& camera) {
		camera.wvp_data.set_cb(material.component->pipeline.root_signature.get_constant_buffer("WVP_BUFFER"));
		transform_data.set_cb(material.component->pipeline.root_signature.get_constant_buffer("TRANSFORM_BUFFER"));
		lights_data.set_cb(material.component->pipeline.root_signature.get_constant_buffer("LIGHTS_BUFFER"));
		camera.pos_data.set_cb(material.component->pipeline.root_signature.get_constant_buffer("CAMERA_BUFFER"));

		material.set_resources();
	}

	void clean_up() {
		lights_data.clean_up();
		transform_data.clean_up();
		material.clean_up();
	}

	bool operator==(StaticMeshComponent component) { return (component == *(this->component)); }

	RenderingMaterial material{};

	ConstantBufferContainer<PSLightsCB> lights_data{};
	ConstantBufferContainer<VSTransformConstants> transform_data{};

	bool update_lights_signal = false;
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
			camera.component->needs_compile(true);
		} else if (component->get_type() == Component::Type::DirectionalLightComponent) {
			directional_lights.push_back(RenderingDirectionalLight{(DirectionalLightComponent*)component});
			directional_lights.back().component->needs_compile(true);
		} else if (component->get_type() == Component::Type::PointLightComponent) {
			point_lights.push_back(RenderingPointLight{(PointLightComponent*)component});
			point_lights.back().component->needs_compile(true);
		} else if (component->get_type() == Component::Type::SpotlightComponent) {
			spotlights.push_back(RenderingSpotlight{(SpotlightComponent*)component});
			spotlights.back().component->needs_compile(true);
		} else if (component->get_type() == Component::Type::StaticMeshComponent) {
			static_meshes.push_back(std::make_shared<RenderingStaticMesh>((StaticMeshComponent*)component));
			static_meshes.back()->component->needs_compile(true);
		} else if (component->get_type() == Component::Type::SkyComponent) {
			sky = RenderingSky{(SkyComponent*)component};
			sky.component->needs_compile(true);
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
		if (camera.component->needs_compile())
			camera.compile();

		if (sky.component != nullptr && sky.component->needs_compile()) {
			sky.compile(camera);
		}

		for (RenderingDirectionalLight &dl : directional_lights) {
			if (dl.component->needs_compile())
				dl.compile();
		}
		for (RenderingPointLight &pl : point_lights) {
			if (pl.component->needs_compile())
				pl.compile();
		}
		for (RenderingSpotlight &sl : spotlights) {
			if (sl.component->needs_compile())
				sl.compile();
		}

		for (auto &mesh : static_meshes) {
			if (camera.component != nullptr && mesh->component->needs_compile()) {
				mesh->compile(camera);
				mesh->update_lights(directional_lights, point_lights, spotlights);
			}
		}
	}

	void set_resources() {
		if (sky.component != nullptr) {
			sky.set_resources(camera);
		}

		for (auto& mesh : static_meshes) {
			if (camera.component != nullptr) {
				mesh->set_resources(camera);
			}
		}
	}

	/**
	 * Look for changes in component data and update graphics-side data accordingly.
	 * 
	 * \param resolution Render resolution. Needed for updating camera.
	 */
	void update(UVector2 resolution) {
		bool cam_update{camera.update(resolution)};

		if (sky.component != nullptr)
			sky.update(camera);

		bool light_update = false;
		for (RenderingDirectionalLight &dl : directional_lights) {
			bool update = dl.update();
			if (!light_update) light_update = true;
		}
		for (RenderingPointLight &pl : point_lights) {
			bool update = pl.update();
			if (!light_update) light_update = true;
		}
		for (RenderingSpotlight &sl : spotlights) {
			bool update = sl.update();
			if (!light_update) light_update = true;
		}

		bool mesh_update{false};
		for (auto &mesh : static_meshes) {
			if (mesh->update()) mesh_update = true;
			if (light_update || mesh->update_lights_signal) mesh->update_lights(directional_lights, point_lights, spotlights);
		}

		camera.component->has_changed(false);

		/*if (cam_update || mesh_update) {
			// Sort list for opaque, distant non-opaque, closeer non-opaque
			static int i{0};
			//std::cout << "sort " << i++ << '\n';
			std::ranges::sort(static_meshes, [&](const auto& s1, const auto& s2) {
				if (s1->component->get_material().is_opaque() ||
					s2->component->get_material().is_opaque()) return true;
				
				bool b{distance(s1->component->get_position(), camera.component->get_position()) >
					distance(s2->component->get_position(), camera.component->get_position())};
				//std::cout << std::boolalpha << b << '\n';

				return b;
			});
		}*/
	}

	void clean_up();

private:
	friend class Renderer;

	std::vector<std::shared_ptr<RenderingStaticMesh>> static_meshes{};
	std::vector<RenderingDirectionalLight> directional_lights{};
	std::vector<RenderingPointLight> point_lights{};
	std::vector<RenderingSpotlight> spotlights{};
	RenderingCamera camera{};
	RenderingSky sky{};
};
