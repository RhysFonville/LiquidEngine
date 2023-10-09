#pragma once

#include "Object.h"
#include "KeybindSet.h"

class Character : public Object {
public:
	KeybindSet keybinds;
};

