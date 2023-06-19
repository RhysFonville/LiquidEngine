#pragma once

#include "ObjectBehavior.h"

class MyBehavior : public ObjectBehavior {
public:
	MyBehavior(Object* object);

	void tick(float dt) override;
};
