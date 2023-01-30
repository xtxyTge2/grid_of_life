#pragma once

//--------------------------------------------------------------------------------
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "state_render_data.hpp"

enum Grid_UI_Control_Button_Events {
	GRID_NO_BUTTON_PRESSED,
	GRID_RESET_BUTTON_PRESSED,
	GRID_NEXT_ITERATION_BUTTON_PRESSED,
	GRID_START_STOP_BUTTON_PRESSED
};

struct Grid_UI_Controls_Info {
	Grid_UI_Control_Button_Events button_type = GRID_NO_BUTTON_PRESSED;
	float grid_speed_slider_value = 100.0f;
};

//--------------------------------------------------------------------------------
class UI_State {
public:
	UI_State();

	void update_render_data(State_Render_Data& state_render_data);

	void add_cell_number(float value);
	//--------------------------------------------------------------------------------
	// data
	Grid_UI_Controls_Info grid_ui_controls_info;

	bool   m_show_demo_window;

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
