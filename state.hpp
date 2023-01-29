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
#include "state_render_data.hpp"
#include "ui_renderer.hpp"

struct Grid_Execution_State {
	bool is_running;
	bool run_manual_next_iteration;
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

class Grid_Manager {
public:
	Grid_Manager();

	void initialise();

	void update(std::vector<UI_Event_Type> event_queue);

	void create_new_grid();

	Grid* grid;
	Grid_Execution_State grid_execution_state;
};

//--------------------------------------------------------------------------------
class State {
public:
	State();

	void initialise(GLFWwindow*);

	void update(std::vector<UI_Event_Type> event_queue);

	void process_input();
	
	void framebuffer_size_callback(int, int);

	State_Render_Data create_render_data();
	//--------------------------------------------------------------------------------
	// data
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Mouse> m_mouse;
	GLFWwindow* m_window;

	std::unique_ptr<Timer> m_timer;
	
	Grid_Manager* grid_manager;
};