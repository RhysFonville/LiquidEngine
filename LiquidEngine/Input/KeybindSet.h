#pragma once

#include <Windows.h>
#include <functional>
#include <string>
#include "KeyCodes.h"

namespace Input {
enum class CallbackEvent {
	IsPressed,
	IsNotPressed
};

/**
 * Main class containing keybind data and options.
 */
class KeybindSet {
public:
	struct Category {
		std::string name;

		struct Action {
			std::string name;

			struct Bind {
				WindowsKeyCode key;
				std::function<void(float)> callback;
				CallbackEvent callback_event;
			};
			std::vector<Bind> binds;
		};
		std::vector<Action> actions;
	};
	std::vector<Category> categories;
};
};

