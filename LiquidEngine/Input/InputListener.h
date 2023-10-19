#pragma once

#include <memory>
#include <algorithm>
#include "KeybindSet.h"

namespace Input {

class InputListener {
public:
	InputListener() { }
	
	void handle_input(float dt);
	void handle_bind(const KeybindSet::Category::Action::Bind &bind, float dt);

	std::vector<KeybindSet*> keybind_sets = { };
};

};