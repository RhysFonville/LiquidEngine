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
	RenderingComponent(const std::weak_ptr<T>& component) : component{component} { }

	bool needs_compile() const noexcept {
		if (auto sp{component.lock()}) return sp->needs_compile();
		else return false;
	}

	void needs_compile(bool b) noexcept {
		if (auto sp{component.lock()}) sp->needs_compile(b);
	}

	bool has_changed() const noexcept {
		if (auto sp{component.lock()}) return sp->has_changed();
		else return false;
	}

	void has_changed(bool b) noexcept {
		if (auto sp{component.lock()}) sp->has_changed(b);
	}

	std::weak_ptr<T> component{};
};

_declspec(align(16))
class RenderingDirectionalLightData/*: DXLight*/ {
public:
	RenderingDirectionalLightData() { }
	RenderingDirectionalLightData(const std::weak_ptr<DirectionalLightComponent>& light) {
		if (auto sp{light.lock()}) {
			albedo = sp->get_albedo().to_vec_normalized();
			specular = sp->get_specular().to_vec_normalized();
			direction = sp->get_rotation();
			null = sp->is_null();
		}
	}

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
	RenderingDirectionalLight(const std::weak_ptr<DirectionalLightComponent>& light) : RenderingComponent{light}, data{light} { }
	
	bool update() {
		if (has_changed()) {
			if (auto sp{component.lock()}) {
				data.albedo = sp->get_albedo().to_vec_normalized();
				data.specular = sp->get_specular().to_vec_normalized();
			
				auto rot = sp->get_rotation();
				data.direction = XMVector3TransformCoord(global_forward.to_xmvec(),
					XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z)
				);

				data.null = sp->is_null();
			}

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
	RenderingPointLightData(const std::weak_ptr<PointLightComponent>& light) {
		if (auto sp{light.lock()}) {
			albedo = sp->get_albedo().to_vec_normalized();
			specular = sp->get_specular().to_vec_normalized();
			range = sp->get_range();
			attenuation = sp->get_attenuation();
			null = sp->is_null();
			position = sp->get_position();
		}
	}

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
	RenderingPointLight(const std::weak_ptr<PointLightComponent>& light) : RenderingComponent{light}, data{light} { }

	bool update() {
		if (has_changed()) {
			if (auto sp{component.lock()}) {
				data.albedo = sp->get_albedo().to_vec_normalized();
				data.specular = sp->get_specular().to_vec_normalized();
				data.range = sp->get_range();
				data.attenuation = sp->get_attenuation();
				data.null = sp->is_null();
				data.position = sp->get_position();
			}

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
	RenderingSpotlightData(const std::weak_ptr<SpotlightComponent>& light) {
		if (auto sp{light.lock()}) {
			albedo = sp->get_albedo().to_vec_normalized();
			specular = sp->get_specular().to_vec_normalized();
			direction = sp->get_rotation();
			null = sp->is_null();
		}
	}

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
	RenderingSpotlight(const std::weak_ptr<SpotlightComponent>& light) : RenderingComponent{light}, data{light} { }

	bool update() {
		if (has_changed()) {
			if (auto sp{component.lock()}) {
				data.albedo = sp->get_albedo().to_vec_normalized();
				data.specular = sp->get_specular().to_vec_normalized();
				data.direction = sp->get_rotation();
			}

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
	RenderingMaterialData(const Material &material)
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
	PSSkyCB(const std::weak_ptr<SkyComponent>& sky) {
	
	}

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
	RenderingCamera(const std::weak_ptr<CameraComponent>& camera)
		: RenderingComponent{camera},
		wvp_data{std::make_shared<VSWVPConstants>()},
		pos_data{std::make_shared<PSCameraConstants>()} { }

	bool update(UVector2 resolution) {
		if (has_changed()) {
			if (auto sp{component.lock()}) {
				sp->update(UVector2_to_FVector2(resolution));
				wvp_data->WVP = XMMatrixTranspose(sp->get_wvp());
				pos_data->camera_position = sp->get_position();
			}

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
	RenderingSky(const std::weak_ptr<SkyComponent>& sky) : RenderingComponent{sky} { }

	bool update(const RenderingCamera& camera) {
		if (auto sp{component.lock()}) {
			if (sp->get_albedo_texture().has_changed()) {
				texture.update();
				texture.compile();
				sp->get_albedo_texture().has_changed(false);
			}
		}

		bool change{false};
		if (camera.has_changed()) change = true;

		if (has_changed()) {
			if (auto sp{component.lock()})
				data.set_obj_ptr(PSSkyCB{sp});
			data.update();

			change = true;
		}

		if (change) {
			if (auto sp{component.lock()}) {
				transform_data.get_obj()->transform = Transform{
					camera.pos_data->camera_position+sp->get_position(),
					sp->get_rotation(),
					sp->get_size()
				};
			}

			has_changed(false);
		}

		return true;
	}

	void compile(RenderingCamera& camera) {
		if (auto sp{component.lock()})
			texture.set_texture(&sp->get_albedo_texture());

		transform_data = VSTransformConstants{};
		data = PSSkyCB{};

		has_changed(true);
		needs_compile(false);
	}

	void set_resources(RenderingCamera& camera) {
		if (auto sp{component.lock()}) {
			wvp_data.set_rc(sp->pipeline.root_signature.get_root_constants("WVP_CONSTANTS"));
			wvp_data.set_obj_ptr(camera.wvp_data);
			
			transform_data.set_rc(sp->pipeline.root_signature.get_root_constants("TRANSFORM_CONSTANTS"));
			data.set_cb(sp->pipeline.root_signature.get_constant_buffer("SKY_BUFFER"));
			texture.set_srv(sp->pipeline.root_signature.get_shader_resource_view("ALBEDO_TEXTURE"));
		}
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
	RenderingMaterial(const std::weak_ptr<Material>& mat) : RenderingComponent{mat} { }

	bool update() {
		bool ret{false};
		if (auto sp{component.lock()}) {
			if (sp->get_albedo_texture().has_changed()) {
				albedo_texture.update();
				albedo_texture.compile();
				sp->get_albedo_texture().has_changed(false);
				ret = true;
			}
			if (sp->get_normal_map().has_changed()) {
				normal_map.update();
				albedo_texture.compile();
				sp->get_normal_map().has_changed(false);
				ret = true;
			}
			if (sp->get_specular_map().has_changed()) {
				specular_map.update();
				albedo_texture.compile();
				sp->get_specular_map().has_changed(false);
				ret = true;
			}
		}

		if (has_changed() || ret) {
			if (auto sp{component.lock()})
				material_data.get_obj()->material = RenderingMaterialData{*sp};

			material_data.update();

			has_changed(false);
			return true;
		}
		return ret;
	}

	void compile() {
		if (auto sp{component.lock()}) {
			albedo_texture.set_texture(&sp->get_albedo_texture());
			normal_map.set_texture(&sp->get_normal_map());
			specular_map.set_texture(&sp->get_specular_map());
		}
		material_data = PSMaterialCB{};

		has_changed(true);
		needs_compile(false);
	}

	void set_resources() {
		if (auto sp{component.lock()}) {
			material_data.set_cb(sp->pipeline.root_signature.get_constant_buffer("OBJECT_BUFFER"));
			albedo_texture.set_srv(sp->pipeline.root_signature.get_shader_resource_view("ALBEDO_TEXTURE"));
			normal_map.set_srv(sp->pipeline.root_signature.get_shader_resource_view("NORMAL_MAP"));
			specular_map.set_srv(sp->pipeline.root_signature.get_shader_resource_view("SPECULAR_MAP"));
		}
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
	RenderingStaticMesh(const std::weak_ptr<StaticMeshComponent>& smc) : RenderingComponent{smc} { }

	bool update(RenderingCamera& camera) {
		bool ret{false};
		if (material.update()) ret = true;
		
		if (auto sp{component.lock()}) {
			if (camera.has_changed() && sp->get_lod_meshes().size() > 1) {
				if (auto camera_sp{camera.component.lock()}) {
					auto it{sp->get_mesh_for_point(camera_sp->get_position())};
					sp->get_current_mesh() = it;
				}
			
				sp->get_material().lock()->pipeline.input_assembler.set_vertex_buffer_view_to_draw(
					std::distance(
						sp->get_lod_meshes().begin(),
						std::map<float, Mesh>::const_iterator{sp->get_current_mesh()}
					)
				);
			}
		}

		if (has_changed()) {
			if (auto sp{component.lock()})
				transform_data.get_obj()->transform = sp->get_transform();
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
		if (auto sp{component.lock()})
			material = RenderingMaterial{sp->get_material()};

		lights_data = PSLightsCB{};
		transform_data = VSTransformConstants{};
		
		material.compile();

		has_changed(true);
		needs_compile(false);
	}

	void set_resources(RenderingCamera& camera) {
		if (auto sp{material.component.lock()}) {
			wvp_data.set_rc(sp->pipeline.root_signature.get_root_constants("WVP_CONSTANTS"));
			wvp_data.set_obj_ptr(camera.wvp_data);
		
			pos_data.set_rc(sp->pipeline.root_signature.get_root_constants("CAMERA_CONSTANTS"));
			pos_data.set_obj_ptr(camera.pos_data);

			transform_data.set_rc(sp->pipeline.root_signature.get_root_constants("TRANSFORM_CONSTANTS"));
			lights_data.set_cb(sp->pipeline.root_signature.get_constant_buffer("LIGHTS_BUFFER"));

			material.set_resources();
		}
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

template<class T, class U>
static std::weak_ptr<T> static_pointer_cast(std::weak_ptr<U> const& r) {
	return std::static_pointer_cast<T>(std::shared_ptr<U>(r));
}

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
	void add_component(const std::weak_ptr<T>& component) {
		if (auto sp{component.lock()}) {
			if (std::dynamic_pointer_cast<CameraComponent>(sp) != nullptr) {
				camera = RenderingCamera{static_pointer_cast<CameraComponent>(component)};
				camera.needs_compile(true);
			} else if (std::dynamic_pointer_cast<DirectionalLightComponent>(sp) != nullptr) {
				directional_lights.push_back(RenderingDirectionalLight{static_pointer_cast<DirectionalLightComponent>(component)});
				directional_lights.back().needs_compile(true);
			} else if (std::dynamic_pointer_cast<PointLightComponent>(sp) != nullptr) {
				point_lights.push_back(RenderingPointLight{static_pointer_cast<PointLightComponent>(component)});
				point_lights.back().needs_compile(true);
			} else if (std::dynamic_pointer_cast<SpotlightComponent>(sp) != nullptr) {
				spotlights.push_back(RenderingSpotlight{static_pointer_cast<SpotlightComponent>(component)});
				spotlights.back().needs_compile(true);
			} else if (std::dynamic_pointer_cast<StaticMeshComponent>(sp) != nullptr) {
				static_meshes.push_back(std::make_shared<RenderingStaticMesh>(static_pointer_cast<StaticMeshComponent>(component)));
				static_meshes.back()->needs_compile(true);
			} else if (std::dynamic_pointer_cast<SkyComponent>(sp) != nullptr) {
				sky = RenderingSky{static_pointer_cast<SkyComponent>(component)};
				sky.needs_compile(true);
			}
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
		if (camera.needs_compile())
			camera.compile();

		if (!sky.component.expired() && sky.needs_compile()) {
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
			if (!camera.component.expired() && mesh->needs_compile()) {
				mesh->compile(camera);
				mesh->update_lights(directional_lights, point_lights, spotlights);
			}
		}
	}

	void set_resources() {
		if (!sky.component.expired()) {
			sky.set_resources(camera);
		}

		for (auto& mesh : static_meshes) {
			if (!camera.component.expired()) {
				mesh->set_resources(camera);
			}
		}
	}

	void refresh_pipelines(const ComPtr<ID3D12Device>& device, const DXGI_SAMPLE_DESC& msaa_sample_desc, const D3D12_BLEND_DESC& blend_desc) {
		if (auto sp{sky.component.lock()})
			sp->pipeline.refresh_pipeline(device, msaa_sample_desc, blend_desc);

		for (auto& mesh : static_meshes) {
			if (auto sp{mesh->material.component.lock()})
				sp->pipeline.refresh_pipeline(device, msaa_sample_desc, blend_desc);
		}
	}

	/**
	 * Look for changes in component data and update graphics-side data accordingly.
	 * 
	 * \param resolution Render resolution. Needed for updating camera.
	 */
	void update(UVector2 resolution) {
		bool cam_update{camera.update(resolution)};

		if (!sky.component.expired())
			sky.update(camera);

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
			if (mesh->update(camera)) mesh_update = true;
			if (light_update || mesh->update_lights_signal) mesh->update_lights(directional_lights, point_lights, spotlights);
		}

		camera.has_changed(false);

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
