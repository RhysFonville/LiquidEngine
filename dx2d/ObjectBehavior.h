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
	virtual void on_start() { }

	virtual void tick() { }

private:
	friend class BehaviorManager;

	void base_on_start();

	void base_tick();

	INT64 object_index = -1;
};

