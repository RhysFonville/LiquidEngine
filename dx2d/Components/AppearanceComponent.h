#pragma once

#include "GraphicsPipeline.h"
#include "MeshComponent.h"
#include "Material.h"



class AppearanceComponent : public Component {
public:
	

	static const Type component_type = Type::AppearanceComponent;

private:
	MeshComponent* mesh = nullptr;
};
