#include "Scene.h"

Scene::Scene(Window &window) {
	objects = std::make_shared<std::vector<std::shared_ptr<Object>>>();
	graphics_scene = std::make_shared<GraphicsScene>(window.get_window(), objects);
	physics_scene = PhysicsScene();
	behavior_manager = BehaviorManager();

	behavior_manager.start();
}

void Scene::tick() {
	behavior_manager.tick();
	graphics_scene->clear();
	graphics_scene->draw();
	graphics_scene->present();
}

void Scene::clean_up() {
	graphics_scene->clean_up();
	//physics_scene.clean_up();
	behavior_manager.clean_up();
}

void Scene::read_obj_file(std::string obj_file_path) noexcept {
	if (!obj_file_path.empty()) {
		std::ifstream obj_file(obj_file_path);
		std::ifstream mtl_file;

		std::vector<std::string> file_vec;

		std::string line;

		// Find name of .mtl file
		if (obj_file.is_open()) {
			while (std::getline(obj_file, line)) {
				if (line.substr(0, 6) == "mtllib") {
					mtl_file.open(line.substr(7));
					break;
				}
			}
		} else {
			ERROR_MESSAGE(L"Could not open file \"" + string_to_wstring(obj_file_path) + L"\"");
		}

		// Read .mtl file
		if (mtl_file.is_open()) {
			bool first_mtl = true;
			int last_new_mtl_index = 0;

			Material newmtl;

			while (std::getline(mtl_file, line)) {
				if (line.length() >= 14) {
					if (line.substr(2, 14) == "Material Count") {
						Storage::materials.reserve(line[line.length()-1] - '0');
					}
				}

				if (line.substr(0, 6) == "newmtl") {
					if (!first_mtl) {
						newmtl.read_mtl_file(std::vector<std::string>(file_vec.begin()+last_new_mtl_index,
							file_vec.end()));

						last_new_mtl_index = file_vec.size();

						Storage::materials.push_back(newmtl);
					}

					newmtl = Material(line.substr(7));
					first_mtl = false;
				}

				file_vec.push_back(line);
			}

			newmtl.read_mtl_file(std::vector<std::string>(file_vec.begin()+last_new_mtl_index,
				file_vec.end()));

			Storage::materials.push_back(newmtl);
		} else {
			ERROR_MESSAGE(L"Could not open file \"" + string_to_wstring(obj_file_path) + L"\"");
		}
		file_vec.clear();

		// Read rest of .obj file
		if (obj_file.is_open()) {
			bool first_obj_sight = true;
			size_t last_new_obj_index = 0;

			Object obj;

			ReadObjFileDataOutput mesh_data;

			std::string line;
			while (std::getline(obj_file, line)) {
				if (line[0] == 'o') {
					if (!first_obj_sight) {
						mesh_data += obj.read_obj_file(std::vector<std::string>(file_vec.begin() + last_new_obj_index,
							file_vec.end()-1), mesh_data);

						objects->push_back(std::make_shared<Object>(obj));

						last_new_obj_index = file_vec.size();
					}
					obj = Object(line.substr(2));
					first_obj_sight = false;
				}
				file_vec.push_back(line);
			}

			mesh_data += obj.read_obj_file(std::vector<std::string>(file_vec.begin()+last_new_obj_index,
				file_vec.end()), mesh_data);

			objects->push_back(std::make_shared<Object>(obj));

		} else {
			ERROR_MESSAGE(L"Could not open file \"" + string_to_wstring(obj_file_path) + L"\"");
		}
	}
}
