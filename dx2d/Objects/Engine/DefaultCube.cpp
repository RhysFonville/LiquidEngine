#include "DefaultCube.h"

DefaultCube::DefaultCube() : Object() {
	add_component(StaticMeshComponent());
}
