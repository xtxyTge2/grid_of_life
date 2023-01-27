#pragma once

#include "Tracy.hpp"
//--------------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <unordered_map>
#include <array>

#include "opengl.hpp"
#include "camera.hpp"
#include "shader_program.hpp"
#include "read.hpp"
#include "texture.hpp"
#include "cube.hpp"
#include "grid.hpp"


//--------------------------------------------------------------------------------
class State_Render_Data {
public:
	State_Render_Data();
	//--------------------------------------------------------------------------------
	// data
	Camera_Render_Data* camera_render_data;

	Grid_Render_Data* grid_render_data;
};

//--------------------------------------------------------------------------------
class Timer {
public:
	Timer();

	void update();
	//--------------------------------------------------------------------------------
	// data
	float m_delta_time;
	float m_last_frame_time;
};

//--------------------------------------------------------------------------------
class State {
public:
	State();

	void initialise(GLFWwindow*);

	void update();

	void process_input();
	
	void framebuffer_size_callback(int, int);

	State_Render_Data create_render_data();

	void reset_grid();
	//--------------------------------------------------------------------------------
	// data
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Mouse> m_mouse;
	GLFWwindow* m_window;

	std::unique_ptr<Timer> m_timer;
	
	Grid* grid;
};