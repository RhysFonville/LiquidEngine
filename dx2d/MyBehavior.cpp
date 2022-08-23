#include "MyBehavior.h"

MyBehavior::MyBehavior(const ObjectVector &objects, std::shared_ptr<Object> &object)
	: ObjectBehavior(objects, object, "MyBehavior") {
	object->get_component<MeshComponent>()->material.ks = 0.5f;
	object->get_component<MeshComponent>()->material.kd = 0.5f;
	object->get_component<MeshComponent>()->material.ka = 0.5f;
	object->get_component<MeshComponent>()->material.a = 0.5f;
}

void MyBehavior::tick() {
	//object->rotate({ 0.01f, 0.01f, 0.01f });

	/*std::shared_ptr<MeshComponent> mesh_component = object->get_component<MeshComponent>();
	if (compf(mesh_component->material.specular, 1.0f))
		going_up = false;
	else if (compf(mesh_component->material.specular, 0.0f))
		going_up = true;

	if (going_up) {
		mesh_component->material.specular += 0.01f;
		mesh_component->material.shininess += 0.01f;
	} else {
		mesh_component->material.specular -= 0.01f;
		mesh_component->material.shininess -= 0.01f;
	}*/
}
