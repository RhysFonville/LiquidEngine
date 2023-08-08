#include "DefaultCube.h"

DefaultCube::DefaultCube() : Object() {
	root_component.add_component(StaticMeshComponent());
}
