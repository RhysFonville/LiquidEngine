#include "Scene.h"

Scene::Scene(Window &window) {
	objects = std::vector<std::shared_ptr<Object>>();

	graphics_scene = GraphicsScene(window.get_window(), std::vector<StaticMeshComponent*>());
	//physics_scene = PhysicsScene(objects);
}

void Scene::tick() {
	dt.tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsed_time = dt.tp2 - dt.tp1;
	dt.tp1 = dt.tp2;
	dt.dt = elapsed_time.count();

	//physics_scene.tick();
	graphics_scene.tick();
}

void Scene::clean_up() {
	//graphics_scene.clean_up();
	//physics_scene.clean_up();
}

void Scene::compile() {
	for (std::shared_ptr<Object> &object : objects) {
		object->compile();
	}

	/*std::vector<StaticMeshComponent*> static_meshes;
	for (std::shared_ptr<Object> &object : objects) {
		if (StaticMeshComponent *appearance = object->get_component<StaticMeshComponent>(); appearance != nullptr) {
			static_meshes.push_back(appearance);
		}
	}
	graphics_scene.static_meshes = static_meshes;*/

	graphics_scene.compile();
}

//void Scene::read_obj_file(std::string obj_file_path) {
//	if (!obj_file_path.empty()) {
//		std::ifstream obj_file(obj_file_path);
//		std::ifstream mtl_file;
//
//		std::string mtl_file_path = "";
//
//		std::vector<std::string> file_vec;
//
//		std::string line = "";
//
//		std::string parent_dir;
//
//		if (!fs::equivalent(obj_file_path, get_parent_directory(obj_file_path, false))) {
//			parent_dir = get_parent_directory(obj_file_path, true);
//		}
//
//		// Find name of .mtl file
//		if (obj_file.is_open()) {
//			while (std::getline(obj_file, line)) {
//				if (!line.empty()) {
//					trim(line);
//					if (line[0] != '#') {
//						remove_extra_spaces(line);
//
//						if (line.substr(0, 6) == "mtllib") {
//							mtl_file_path = parent_dir + line.substr(7);
//							mtl_file.open(mtl_file_path);
//							break;
//						}
//					}
//				}
//			}
//		} else {
//			throw std::exception(("Could not open file \"" + obj_file_path + "\"").c_str());
//		}
//
//		// Read .mtl file
//		if (mtl_file.is_open()) {
//			bool first_mtl = true;
//			size_t last_new_mtl_index = 0;
//
//			Material newmtl;
//
//			while (std::getline(mtl_file, line)) {
//				if (!line.empty()) {
//					trim(line);
//					if (line[0] != '#') {
//						remove_extra_spaces(line);
//
//						if (line.substr(0, 6) == "newmtl") {
//							if (!first_mtl) {
//								newmtl.read_mtl_file(std::vector<std::string>(file_vec.begin()+last_new_mtl_index,
//									file_vec.end()));
//
//								last_new_mtl_index = file_vec.size();
//							}
//
//							newmtl = Material();
//							first_mtl = false;
//						}
//
//						file_vec.push_back(line);
//					}
//				}
//			}
//
//			newmtl.read_mtl_file(std::vector<std::string>(file_vec.begin()+last_new_mtl_index,
//				file_vec.end()));
//		} else {
//			throw std::exception(("Could not open file \"" + mtl_file_path + "\"").c_str());
//		}
//		file_vec.clear();
//
//		// Read rest of .obj file
//		if (obj_file.is_open()) {
//			bool first_obj_sight = true;
//			size_t last_new_obj_index = 0;
//
//			Object obj;
//
//			ReadObjFileDataOutput mesh_data;
//
//			std::string line;
//			while (std::getline(obj_file, line)) {
//				if (!line.empty()) {
//					trim(line);
//					if (line[0] != '#') {
//						remove_extra_spaces(line);
//
//						if (line[0] == 'o') {
//							if (!first_obj_sight) {
//								mesh_data += obj.read_obj_file(std::vector<std::string>(file_vec.begin() + last_new_obj_index,
//									file_vec.end()-1), mesh_data);
//
//								objects.push_back(std::make_shared<Object>(obj));
//
//								last_new_obj_index = file_vec.size();
//							}
//							obj = Object(line.substr(2));
//							first_obj_sight = false;
//						}
//						file_vec.push_back(line);
//					}
//				}
//			}
//
//			mesh_data += obj.read_obj_file(std::vector<std::string>(file_vec.begin()+last_new_obj_index,
//				file_vec.end()), mesh_data);
//
//			objects.push_back(std::make_shared<Object>(obj));
//
//		} else {
//			throw std::exception(("Could not open file \"" + obj_file_path + "\"").c_str());
//		}
//	}
//}

std::vector<Object> & Scene::get_objects() noexcept {
	return objects;
}

void Scene::add_object(Object object) noexcept {
	objects.push_back(object);
}

void Scene::remove_object(int index) noexcept {
	objects.erase(objects.begin()+index);
}

float Scene::get_delta_time() const noexcept {
	return dt.dt;
}
