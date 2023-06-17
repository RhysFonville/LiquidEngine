//#include "ObjectBehavior.h"
//
//ObjectBehavior::ObjectBehavior(const std::vector<std::unique_ptr<Object>> &objects, Object* object, const std::string &name)
//	: name(name), object(object) {
//	for (int i = 0; i < objects.size(); i++) {
//		if (objects[i].get() == object) {
//			object_index = i;
//		}
//	}
//}
//
//void ObjectBehavior::base_on_start() {
//	on_start();
//}
//
//void ObjectBehavior::base_tick() {
//	tick();
//}
