#pragma once

class Controllable {
public:
	virtual void base_clean_up() { clean_up(); }
	virtual void base_compile() { compile(); }
	virtual void base_tick(float dt) { tick(dt); }

	virtual void on_start() { }
	virtual void clean_up() { }
	virtual void compile() { }
	virtual void tick(float dt) { }
};

