#include "InputListener.h"

namespace Input {

void InputListener::handle_input(float dt) {
	for (const KeybindSet* keybind : keybind_sets) {
		if (keybind != nullptr) {
			for (const KeybindSet::Category &category : keybind->categories) {
				for (const KeybindSet::Category::Action &action : category.actions) {
					for (const KeybindSet::Category::Action::Bind &bind : action.binds) {
						handle_bind(bind, dt);
					}
				}
			}
		} else {
			keybind_sets.erase(std::ranges::find_if(keybind_sets, [&](const auto &kb) {
				return (kb == keybind);
			})); // Find and erase
		}
	}
}

void InputListener::handle_bind(const KeybindSet::Category::Action::Bind &bind, float dt) {
	if ((GetAsyncKeyState((int)bind.key) & 0x8000)) {
		if (bind.callback_event == CallbackEvent::IsPressed) {
			bind.callback(dt);
		}
	} else if (bind.callback_event == CallbackEvent::IsNotPressed) {
		bind.callback(dt);
	}
}

};