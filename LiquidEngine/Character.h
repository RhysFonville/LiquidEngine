#pragma once

#include "Object.h"
#include "Input/KeybindSet.h"

class Character : public Object {
public:
	Input::KeybindSet keybind_set;

};

