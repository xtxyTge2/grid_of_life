#pragma once

//--------------------------------------------------------------------------------
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "state.hpp"

//--------------------------------------------------------------------------------
class UI_State {
public:
	UI_State();

	void update_render_data(State_Render_Data& state_render_data);

	void add_cell_number(float value);
	//--------------------------------------------------------------------------------
	// data
	bool   m_update_grid;
	float  m_grid_update_speed;
	bool   m_show_demo_window;
	bool   m_grid_is_running;
	bool   m_grid_should_reset;
	bool   m_show_grid_info;
	constexpr static int m_cell_number_values_size = 100;
	float  m_cell_number_values[m_cell_number_values_size];
	int    m_current_index;
};

//--------------------------------------------------------------------------------
class UI_Renderer {
public:
	UI_Renderer();

	~UI_Renderer();

	void initialise(GLFWwindow* window);

	void render_frame(State_Render_Data& state_render_data);

	bool imgui_wants_to_capture_io();
	//--------------------------------------------------------------------------------
	// data
	GLFWwindow* m_window;
	UI_State* m_ui_state;
};
