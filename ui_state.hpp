#pragma once

//--------------------------------------------------------------------------------
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum Grid_UI_Control_Button_Events {
	GRID_NO_BUTTON_PRESSED,
	GRID_RESET_BUTTON_PRESSED,
	GRID_NEXT_ITERATION_BUTTON_PRESSED,
	GRID_START_STOP_BUTTON_PRESSED
};

struct Grid_UI_Controls_Info {
	bool m_show_demo_window = false;
	bool m_show_grid_info = true;

	Grid_UI_Control_Button_Events button_type = GRID_NO_BUTTON_PRESSED;
	float min_grid_speed_slider_value = 1.0f;
	float max_grid_speed_slider_value = 100.0f;
	float grid_speed_slider_value = max_grid_speed_slider_value;

	bool show_chunk_borders = false;
	bool run_grid_at_max_possible_speed = true;

	int min_number_of_grid_iterations_per_single_frame = 1;
	int max_number_of_grid_iterations_per_single_frame = 100;
	int number_of_grid_iterations_per_single_frame = 1;
};

//--------------------------------------------------------------------------------
struct Grid_Info {
	int rows;
	int columns;

	int origin_row; 
	int origin_column;
	int number_of_alive_cells;

	int iteration;
};


//--------------------------------------------------------------------------------
class UI_State {
public:
	UI_State();

	~UI_State();

	void update(const Grid_Info& grid_info);

	void setup_ui_for_current_frame(const Grid_Info& grid_info);
	
	void initialise(GLFWwindow* window);

	bool wants_to_capture_io();
	//--------------------------------------------------------------------------------
	// data

	Grid_UI_Controls_Info ui_info;
	GLFWwindow* m_window;
};