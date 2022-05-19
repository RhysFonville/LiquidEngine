#include "MyBehavior.h"

//MyBehavior::MyBehavior(const std::shared_ptr<Object> &object) : ObjectBehavior()
//{
//	this->object = object.get();
//	this->name = "MyBehavior";
//}

MyBehavior::MyBehavior(Object &object) : ObjectBehavior() {
	this->object = &object;
	this->name = "MyBehavior";
}

void MyBehavior::tick() {
	object->transform.rotation.x += 0.01f;
	object->transform.rotation.y -= 0.01f;
	object->transform.rotation.z += 0.01f;
}
