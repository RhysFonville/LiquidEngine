#pragma once

/**
 * Base class for everything that needs compile, clean up, and tick functionality.
 * \see Object
 * \see Component
 */
class Controllable {
public:
	virtual void base_clean_up() { clean_up(); }
	virtual void base_compile() { compile(); }
	virtual void base_tick(float dt) { tick(dt); }
	virtual void base_render_editor_gui_section() { render_editor_gui_section(); }

	virtual void pre_scene_compile() { }
	virtual void post_scene_compile() { }
	virtual void clean_up() { }
	virtual void compile() { }
	virtual void tick(float dt) { }
	virtual void render_editor_gui_section() { }
};

