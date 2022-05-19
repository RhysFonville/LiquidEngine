#include "Object.h"

Object::Object() : appearance(ObjectAppearance()), transform(Transform()) {}
Object::Object(ObjectAppearance appearance) : appearance(appearance) { }
