#pragma once

#include "Object.h"
#include "globalutil.h"

class ObjectBehavior {
public:
	ObjectBehavior(Object* object);

	virtual void tick(float dt) = 0;

	Object* object;
};

