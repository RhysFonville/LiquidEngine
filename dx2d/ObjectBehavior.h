#pragma once

#include <memory>
#include "Object.h"
#include "globalutil.h"

class ObjectBehavior {
public:
	ObjectBehavior(const ObjectVector &objects, const std::shared_ptr<Object> &object, const std::string &name);

	std::string name;

	std::shared_ptr<Object> object;

protected:
	virtual void tick() { }

private:
	friend class BehaviorManager;

	void base_tick();

	INT64 object_index = -1;
};

