#pragma once

#include "Object.h"
#include <memory>

class ObjectBehavior {
public:
	ObjectBehavior();

	virtual void tick() { }

	std::string name;
	//std::shared_ptr<Object> object;

	Object *object;
};

