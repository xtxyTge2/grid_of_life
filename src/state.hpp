#pragma once

#include "Tracy.hpp"
//--------------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <unordered_map>
#include <array>

#include "opengl.hpp"
#include "camera.hpp"
#include "read.hpp"
#include "texture.hpp"
#include "cube.hpp"
#include "grid.hpp"

#include "renderer.hpp"


//--------------------------------------------------------------------------------
class Timer {
public:
	Timer();

	void update();
	//--------------------------------------------------------------------------------
	// data
	float dt;
	float m_last_frame_time;
};


class State {
public:
	State();

	void initialise(GLFWwindow* w);

	void update();

	void render_frame();

	void framebuffer_size_callback(int width, int height);

	void scroll_callback(double xoffset, double yoffset);

	bool should_quit();
	//--------------------------------------------------------------------------------
	// data
	GLFWwindow* window;
	std::unique_ptr<Timer> timer;
	std::unique_ptr<UI_State> ui_state;
	std::unique_ptr<Renderer> renderer;
	std::shared_ptr<World> world;
	std::shared_ptr<Cube_System> cube_system;
};