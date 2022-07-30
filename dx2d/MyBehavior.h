#pragma once

#include "ObjectBehavior.h"

class MyBehavior : public ObjectBehavior {
public:
	MyBehavior(const ObjectVector &objects, std::shared_ptr<Object> &object);

	void tick() override;
};
