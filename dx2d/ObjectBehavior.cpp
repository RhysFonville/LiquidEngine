#include "ObjectBehavior.h"

ObjectBehavior::ObjectBehavior(const ObjectVector &objects, const std::shared_ptr<Object> &object, const std::string &name)
	: object_index(std::distance(objects.begin(), std::find(objects.begin(), objects.end(), object))),
	  name(name), object(object) { }

void ObjectBehavior::base_on_start() {
	on_start();
}

void ObjectBehavior::base_tick() {
	tick();
}
