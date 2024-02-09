#pragma once

#include <memory>
#include <algorithm>
#include "KeybindSet.h"

namespace Input {

/**
 * Listens for input and invokes keybind functions.
 * \see KeybindSet
 */
class InputListener {
public:
	InputListener() { }
	
	void handle_input(float dt);
	void handle_bind(const KeybindSet::Category::Action::Bind &bind, float dt);

	std::vector<KeybindSet*> keybind_sets = { };
};

};