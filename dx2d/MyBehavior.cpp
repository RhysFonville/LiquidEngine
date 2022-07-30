#include "MyBehavior.h"

MyBehavior::MyBehavior(const ObjectVector &objects, std::shared_ptr<Object> &object)
	: ObjectBehavior(objects, object, "MyBehavior") { }

void MyBehavior::tick() {
	object->rotate({ 0.01f, 0.01f, 0.01f });
}
