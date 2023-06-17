//#pragma once
//
//#include <memory>
//#include "Object.h"
//#include "globalutil.h"
//
//class ObjectBehavior {
//public:
//	ObjectBehavior(const std::vector<std::unique_ptr<Object>> &objects, Object* object, const std::string &name);
//
//	std::string name;
//
//	Object* object;
//
//
//protected:
//	virtual void on_start() { }
//
//	virtual void tick() { }
//
//private:
//	friend class BehaviorManager;
//
//	void base_on_start();
//
//	void base_tick();
//
//	INT64 object_index = -1;
//};
//
