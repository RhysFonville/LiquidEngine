#pragma once

#include "../../Object.h"
#include "../../Components/StaticMeshComponent.h"

class DefaultCube : public Object {
public:
	DefaultCube() { }

	void on_start() override;
};

