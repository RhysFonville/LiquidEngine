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
template <typename T> requires std::derived_from<T, GraphicsTracker>
class RenderingComponent {
public:
	RenderingComponent() { }
	RenderingComponent(T* component) : component{component} { }

	bool operator==(const T* component) const noexcept { return this->component == component; }

	bool needs_compile() const noexcept {
		return component->needs_compile();
	}

	void needs_compile(bool b) noexcept {
		component->needs_compile(b);
	}

	bool has_changed() const noexcept {
		return component->has_changed();
	}

	void has_changed(bool b) noexcept {
		component->has_changed(b);
	}

	bool valid() const noexcept { return component != nullptr; }

	T* component{};
};

_declspec(align(16))
class RenderingDirectionalLightData/*: DXLight*/ {
public:
	RenderingDirectionalLightData() { }
	RenderingDirectionalLightData(const DirectionalLightComponent* light)
		: albedo{light->get_albedo().to_vec_normalized()},
		specular{light->get_specular().to_vec_normalized()},
		direction{light->get_rotation()}, null{light->is_null()} { }

	FVector4 albedo{1.0f, 1.0f, 1.0f, 1.0f};
	FVector4 specular{0.0f, 0.0f, 0.0f, 1.0f};
	FVector3 direction{0.25f, 0.5f, -1.0f};
	int null{true};
};

/**
* Graphics-side directional light.
* \see DirectionalLightComponent
*/
class RenderingDirectionalLight : public RenderingComponent<DirectionalLightComponent> {
public:
	RenderingDirectionalLight(DirectionalLightComponent* light)
		: RenderingComponent{light}, data{light} { }
	
	bool update() {
		if (has_changed()) {
			data.albedo = component->get_albedo().to_vec_normalized();
			data.specular = component->get_specular().to_vec_normalized();
			
			auto rot = component->get_rotation();
			data.direction = XMVector3TransformCoord(global_forward.to_xmvec(),
				XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z)
			);

			data.null = component->is_null();

			has_changed(false);
			return true;
		}
		return false;
	}

	void compile() {
		has_changed(true);
		needs_compile(false);
	}

	RenderingDirectionalLightData data{};
};

_declspec(align(16))
class RenderingPointLightData/*: DXLight*/ {
public:
	RenderingPointLightData() { }
	RenderingPointLightData(const PointLightComponent* light)
		: albedo{light->get_albedo().to_vec_normalized()},
		specular{light->get_specular().to_vec_normalized()},
		range{light->get_range()}, attenuation{light->get_attenuation()},
		null{light->is_null()}, position{light->get_position()} { }

	FVector4 albedo{1.0f, 1.0f, 1.0f, 1.0f};
	FVector4 specular{0.0f, 0.0f, 0.0f, 1.0f};
	float range{100.0f};
	FVector3 attenuation{0.2f, 0.2f, 0.2f};
	int null{false};
	FVector3 position{0.0f, 0.0f, 0.0f};
};

/**
* Graphics-side point light.
* \see PointLightComponent
*/
class RenderingPointLight : public RenderingComponent<PointLightComponent> {
public:
	RenderingPointLight(PointLightComponent* light)
		: RenderingComponent{light}, data{light} { }

	bool update() {
		if (has_changed()) {
			data.albedo = component->get_albedo().to_vec_normalized();
			data.specular = component->get_specular().to_vec_normalized();
			data.range = component->get_range();
			data.attenuation = component->get_attenuation();
			data.null = component->is_null();
			data.position = component->get_position();

			has_changed(false);
			return true;
		}
		return false;
	}

	void compile() {
		has_changed(true);
		needs_compile(false);
	}

	RenderingPointLightData data{};
};

_declspec(align(16))
class RenderingSpotlightData/*: DXLight*/ {
public:
	RenderingSpotlightData() { }
	RenderingSpotlightData(const SpotlightComponent* light)
		: albedo{light->get_albedo().to_vec_normalized()},
		specular{light->get_specular().to_vec_normalized()},
		direction{light->get_rotation()},
		null{light->is_null()} { }

	FVector4 albedo{1.0f, 1.0f, 1.0f, 1.0f};
	FVector4 specular{0.0f, 0.0f, 0.0f, 1.0f};
	FVector3 direction{0.0f, 0.0f, 0.0f};
	int null{false};
};

/**
* Graphics-side spotlight.
* \see SpotlightComponent
*/
class RenderingSpotlight : public RenderingComponent<SpotlightComponent> {
public:
	RenderingSpotlight(SpotlightComponent* light)
		: RenderingComponent{light}, data{light} { }

	bool update() {
		if (has_changed()) {
			data.albedo = component->get_albedo().to_vec_normalized();
			data.specular = component->get_specular().to_vec_normalized();
			data.direction = component->get_rotation();

			has_changed(false);
			return true;
		}
		return false;
	}

	void compile() {
		has_changed(true);
		needs_compile(false);
	}

	RenderingSpotlightData data{};
};

_declspec(align(16))
class RenderingMaterialData {
public:
	RenderingMaterialData() { }
	RenderingMaterialData(const Material& material)
		: has_texture{material.has_texture()}, has_normal_map{material.has_normal_map()},
		has_specular_map{material.has_specular_map()}, a(material.get_shininess()),
		ks(material.get_specular().to_vec_normalized()),
		kd(material.get_albedo().to_vec_normalized()),
		ka(material.get_ambient().to_vec_normalized()) { }

	int has_texture{true};
	int has_normal_map{true};
	int has_specular_map{true};
	float pad{0.0f};
	FVector4 ks{0.3f, 0.3f, 0.3f, 0.3f}; // Specular
	FVector4 kd{1.0f, 1.0f, 1.0f, 1.0f}; // Diffuse
	FVector4 ka{0.0f, 0.0f, 0.0f, 1.0f}; // Ambient
	float a{0.5f}; // Shininess
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
	UINT directional_light_count{0};
	UINT point_light_count{0};
	UINT spotlight_count{0};

	std::array<RenderingDirectionalLightData, MAX_LIGHTS_PER_TYPE> directional_lights{};
	std::array<RenderingPointLightData, MAX_LIGHTS_PER_TYPE> point_lights{};
	std::array<RenderingSpotlightData, MAX_LIGHTS_PER_TYPE> spotlights{};
};

__declspec(align(16))
struct PSCameraConstants { // b3
	FVector3 camera_position{0.0f, 0.0f, 0.0f};
	float pad{0.0f};
};

__declspec(align(16))
struct PSMaterialCB { // b4
	RenderingMaterialData material{};
};

__declspec(align(16))
struct PSSkyCB { // b2
	PSSkyCB() { }
	PSSkyCB(const SkyComponent* sky)
		: has_texture{(int)sky->has_texture()},
		albedo{sky->get_albedo()} { }

	int has_texture{false};
	FVector3 pad{0.0f, 0.0f, 0.0f};
	FVector4 albedo{0.0f, 0.0f, 0.0f, 1.0f};
};

/**
* Graphics-side camera.
* \see CameraComponent
*/
class RenderingCamera : public RenderingComponent<CameraComponent> {
public:
	RenderingCamera() { }
	RenderingCamera(CameraComponent* camera)
		: RenderingComponent{camera},
		wvp_data{std::make_shared<VSWVPConstants>()},
		pos_data{std::make_shared<PSCameraConstants>()} { }

	bool update(const UVector2& resolution) {
		if (has_changed()) {
			component->update(UVector2_to_FVector2(resolution));
			wvp_data->WVP = XMMatrixTranspose(component->get_wvp());
			pos_data->camera_position = component->get_position();

			return true;
		}
		return false;
	}

	void compile() {
		has_changed(true);
		needs_compile(false);
	}

	void clean_up() { }

	std::shared_ptr<VSWVPConstants> wvp_data{};
	std::shared_ptr<PSCameraConstants> pos_data{};
};

/**
* Graphics-side texture.
*/
/*class RenderingTexture : public RenderingComponent<Texture> {
public:
	RenderingTexture() { }
	RenderingTexture(Texture* texture) : RenderingComponent{texture} { }

	bool update() {
		if (has_changed()) {
			update_descs_and_compile_srv();

			has_changed(false);
			return true;
		}
		return false;
	}

	void compile(const std::string& name) {
		camera.wvp_data.rc = component->pipeline.root_signature.get_resource(name);

		needs_compile(false);
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
		if (component->get_albedo_texture().has_changed()) {
			texture.update();
			texture.compile();
			component->get_albedo_texture().has_changed(false);
		}

		bool change{false};
		if (camera.has_changed()) change = true;

		if (has_changed()) {
			data.set_obj_ptr(PSSkyCB{component});
			data.update();

			change = true;
		}

		if (change) {
			transform_data.get_obj()->transform = Transform{
				camera.pos_data->camera_position+component->get_position(),
				component->get_rotation(),
				component->get_size()
			};

			has_changed(false);
		}

		return true;
	}

	void compile(RenderingCamera& camera) {
		texture.set_texture(&component->get_albedo_texture());

		transform_data = VSTransformConstants{};
		data = PSSkyCB{};

		has_changed(true);
		needs_compile(false);
	}

	void set_resources(RenderingCamera& camera) {
		wvp_data.set_rc(component->pipeline.root_signature.get_root_constants("WVP_CONSTANTS"));
		wvp_data.set_obj_ptr(camera.wvp_data);
			
		transform_data.set_rc(component->pipeline.root_signature.get_root_constants("TRANSFORM_CONSTANTS"));
		data.set_cb(component->pipeline.root_signature.get_constant_buffer("SKY_BUFFER"));
		texture.set_srv(component->pipeline.root_signature.get_shader_resource_view("ALBEDO_TEXTURE"));
	}

	void clean_up() {
		texture.clean_up();
		transform_data.clean_up();
		data.clean_up();
	}

	ShaderResourceViewContainer texture{};

	RootConstantsContainer<VSTransformConstants> transform_data{};
	ConstantBufferContainer<PSSkyCB> data{};
	RootConstantsContainer<VSWVPConstants> wvp_data{};
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
		if (component->get_albedo_texture().has_changed()) {
			albedo_texture.update();
			albedo_texture.compile();
			component->get_albedo_texture().has_changed(false);
			ret = true;
		}
		if (component->get_normal_map().has_changed()) {
			normal_map.update();
			albedo_texture.compile();
			component->get_normal_map().has_changed(false);
			ret = true;
		}
		if (component->get_specular_map().has_changed()) {
			specular_map.update();
			albedo_texture.compile();
			component->get_specular_map().has_changed(false);
			ret = true;
		}

		if (has_changed() || ret) {
			material_data.get_obj()->material = RenderingMaterialData{*component};

			material_data.update();

			has_changed(false);
			return true;
		}
		return ret;
	}

	void compile() {
		albedo_texture.set_texture(&component->get_albedo_texture());
		normal_map.set_texture(&component->get_normal_map());
		specular_map.set_texture(&component->get_specular_map());
		material_data = PSMaterialCB{};

		has_changed(true);
		needs_compile(false);
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

	bool update(RenderingCamera& camera) {
		bool ret{false};
		if (material.update()) ret = true;
		
		if (camera.has_changed() && component->get_lod_meshes().size() > 1) {
			auto it{component->get_mesh_for_point(camera.component->get_position())};
			component->get_current_mesh() = it;
			
			component->get_material().lock()->pipeline.input_assembler.set_vertex_buffer_view_to_draw(
				std::distance(
					component->get_lod_meshes().begin(),
					std::map<float, Mesh>::const_iterator{component->get_current_mesh()}
				)
			);
		}

		if (has_changed()) {
			transform_data.get_obj()->transform = component->get_transform();
			has_changed(false);
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
		material = RenderingMaterial{component->get_material().lock().get()};

		lights_data = PSLightsCB{};
		transform_data = VSTransformConstants{};
		
		material.compile();

		has_changed(true);
		needs_compile(false);
	}

	void set_resources(RenderingCamera& camera) {
		wvp_data.set_rc(material.component->pipeline.root_signature.get_root_constants("WVP_CONSTANTS"));
		wvp_data.set_obj_ptr(camera.wvp_data);
		
		pos_data.set_rc(material.component->pipeline.root_signature.get_root_constants("CAMERA_CONSTANTS"));
		pos_data.set_obj_ptr(camera.pos_data);

		transform_data.set_rc(material.component->pipeline.root_signature.get_root_constants("TRANSFORM_CONSTANTS"));
		lights_data.set_cb(material.component->pipeline.root_signature.get_constant_buffer("LIGHTS_BUFFER"));

		material.set_resources();
	}

	void clean_up() {
		lights_data.clean_up();
		transform_data.clean_up();
		material.clean_up();
	}

	RenderingMaterial material{};

	RootConstantsContainer<VSWVPConstants> wvp_data{};
	RootConstantsContainer<PSCameraConstants> pos_data{};
	RootConstantsContainer<VSTransformConstants> transform_data{};
	ConstantBufferContainer<PSLightsCB> lights_data{};

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
	template <typename T> requires std::derived_from<T, GraphicsComponent>
	void add_component(T* component) {
		if (dynamic_cast<CameraComponent*>(component) != nullptr) {
			camera = RenderingCamera{static_cast<CameraComponent*>(component)};
			camera.needs_compile(true);
		} else if (dynamic_cast<DirectionalLightComponent*>(component) != nullptr) {
			directional_lights.push_back(RenderingDirectionalLight{static_cast<DirectionalLightComponent*>(component)});
			directional_lights.back().needs_compile(true);
		} else if (dynamic_cast<PointLightComponent*>(component) != nullptr) {
			point_lights.push_back(RenderingPointLight{static_cast<PointLightComponent*>(component)});
			point_lights.back().needs_compile(true);
		} else if (dynamic_cast<SpotlightComponent*>(component) != nullptr) {
			spotlights.push_back(RenderingSpotlight{static_cast<SpotlightComponent*>(component)});
			spotlights.back().needs_compile(true);
		} else if (dynamic_cast<StaticMeshComponent*>(component) != nullptr) {
			static_meshes.push_back(static_cast<StaticMeshComponent*>(component));
			static_meshes.back().needs_compile(true);
		} else if (dynamic_cast<SkyComponent*>(component) != nullptr) {
			sky = RenderingSky{static_cast<SkyComponent*>(component)};
			sky.needs_compile(true);
		}
	}

	template <typename T> requires std::derived_from<T, GraphicsComponent>
	void remove_component(T* component) {
		if (dynamic_cast<StaticMeshComponent*>(component) != nullptr) {
			static_meshes.erase(std::ranges::find_if(static_meshes, [&](const auto& x){return x == (StaticMeshComponent*)component;}));
		} else if (dynamic_cast<CameraComponent*>(component) != nullptr) {
			if (camera == (CameraComponent*)component) {
				camera = nullptr;
			}
		} else if (dynamic_cast<DirectionalLightComponent*>(component) != nullptr) {
			directional_lights.erase(std::ranges::find_if(directional_lights, [&](const auto& x) {return x == (DirectionalLightComponent*)component; }));
		} else if (dynamic_cast<PointLightComponent*>(component) != nullptr) {
			point_lights.erase(std::ranges::find_if(point_lights, [&](const auto& x) {return x == (PointLightComponent*)component; }));
		} else if (dynamic_cast<SpotlightComponent*>(component) != nullptr) {
			spotlights.erase(std::ranges::find_if(spotlights, [&](const auto& x) {return x == (SpotlightComponent*)component; }));
		}
	}

	void compile() {
		if (camera.valid() && camera.needs_compile())
			camera.compile();

		if (sky.valid() && sky.needs_compile()) {
			sky.compile(camera);
		}

		for (RenderingDirectionalLight &dl : directional_lights) {
			if (dl.needs_compile())
				dl.compile();
		}
		for (RenderingPointLight &pl : point_lights) {
			if (pl.needs_compile())
				pl.compile();
		}
		for (RenderingSpotlight &sl : spotlights) {
			if (sl.needs_compile())
				sl.compile();
		}

		for (auto &mesh : static_meshes) {
			if (mesh.needs_compile()) {
				mesh.compile(camera);
				mesh.update_lights(directional_lights, point_lights, spotlights);
			}
		}
	}

	void set_resources() {
		if (sky.valid()) sky.set_resources(camera);

		for (auto& mesh : static_meshes) {
			mesh.set_resources(camera);
		}
	}

	void refresh_pipelines(const ComPtr<ID3D12Device>& device, const DXGI_SAMPLE_DESC& msaa_sample_desc, const D3D12_BLEND_DESC& blend_desc) {
		if (sky.valid())
			sky.component->pipeline.refresh_pipeline(device, msaa_sample_desc, blend_desc);

		for (auto& mesh : static_meshes) {
			mesh.material.component->pipeline.refresh_pipeline(device, msaa_sample_desc, blend_desc);
		}
	}

	/**
	 * Look for changes in component data and update graphics-side data accordingly.
	 * 
	 * \param resolution Render resolution. Needed for updating camera.
	 */
	void update(UVector2 resolution) {
		bool cam_update{false};
		if (camera.valid()) cam_update = camera.update(resolution);

		if (sky.valid()) sky.update(camera);

		bool light_update = false;
		for (RenderingDirectionalLight &dl : directional_lights) {
			bool update = dl.update();
			if (update) light_update = true;
		}
		for (RenderingPointLight &pl : point_lights) {
			bool update = pl.update();
			if (update) light_update = true;
		}
		for (RenderingSpotlight &sl : spotlights) {
			bool update = sl.update();
			if (update) light_update = true;
		}

		bool mesh_update{false};
		for (auto &mesh : static_meshes) {
			if (mesh.update(camera)) mesh_update = true;
			if (light_update || mesh.update_lights_signal) mesh.update_lights(directional_lights, point_lights, spotlights);
		}

		if (camera.valid()) camera.has_changed(false);

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

	void clean_up() {
		for (auto& mesh : static_meshes) {
			mesh.clean_up();
		}

		if (camera.valid()) camera.clean_up();
		if (sky.valid()) sky.clean_up();
	}

private:
	friend class Renderer;

	std::vector<RenderingStaticMesh> static_meshes{};
	std::vector<RenderingDirectionalLight> directional_lights{};
	std::vector<RenderingPointLight> point_lights{};
	std::vector<RenderingSpotlight> spotlights{};
	RenderingCamera camera{};
	RenderingSky sky{};
};
