#pragma once

#include <Windows.h>
#include <functional>
#include "KeyCodes.h"

namespace Input {
class KeybindSet {
public:
	class Category {
		std::string name;

		class Action {
			std::string name;

			class Bind {
				WindowsKeyCode key;
				std::function<void()> callback;
			};
			std::vector<Bind> binds;
		};
		std::vector<Action> actions;
	};
	std::vector<Category> categories;
};
};

