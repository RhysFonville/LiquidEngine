#pragma once

#include "Object.h"
#include "Input/KeybindSet.h"

/**
 * An object that accepts user input.
 * \see Object
 * \see KeybindSet
 */
class Character : public Object {
public:
	Input::KeybindSet keybind_set;

};

