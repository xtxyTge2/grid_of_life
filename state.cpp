#pragma once

//--------------------------------------------------------------------------------
#include "state.hpp"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
State::State() :
	m_camera(nullptr),
m_mouse(nullptr),
m_window(nullptr),
m_timer(nullptr),
grid_manager(nullptr)
{

}

//--------------------------------------------------------------------------------
void State::initialise(GLFWwindow* window) {
	ZoneScoped;
	m_window = window;

	m_timer = std::make_unique < Timer > ();

	m_mouse = std::make_unique < Mouse > ();
	m_mouse->initialise(0.0f, 0.0f);

	m_camera = std::make_unique < Camera > ();
	m_camera->position = glm::vec3(Chunk::rows / 2, Chunk::columns / 2, 125.0f);

	// left/right/x-axis direction vector
	m_camera->orientation_vector_matrix[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	
	// up/down/y-axis direction vector
	m_camera->orientation_vector_matrix[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	
	// front/back/z-axis direction vector
	m_camera->orientation_vector_matrix[2] = glm::vec3(0.0f, 0.0f, -1.0f);
	
	m_camera->m_speed = 50.0f;
	
	grid_manager = new Grid_Manager();
}


//--------------------------------------------------------------------------------
void State::update(Grid_UI_Controls_Info grid_ui_controls_info) {
	ZoneScoped;

	m_timer->update();
	double dt = m_timer->dt;

	grid_manager->update(dt, grid_ui_controls_info);
}

//--------------------------------------------------------------------------------
void State::framebuffer_size_callback(int width, int height) {
	ZoneScoped;
	glViewport(0, 0, width, height);
}

//--------------------------------------------------------------------------------
void State::process_input() {
	ZoneScoped;
	m_camera->m_speed = 50.0f * m_timer->dt;

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_window, true);
	}
	
	Camera_Move_Direction direction = UNDEFINED;
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
		direction = Camera_Move_Direction::FORWARD;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		direction = Camera_Move_Direction::BACKWARD;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
		direction = Camera_Move_Direction::LEFT;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		direction = Camera_Move_Direction::RIGHT;
	}
	m_camera->move(direction);
}


//--------------------------------------------------------------------------------
State_Render_Data::State_Render_Data() {

}

//--------------------------------------------------------------------------------
State_Render_Data State::create_render_data() {
	ZoneScoped;
	State_Render_Data* render_data = new State_Render_Data();

	// Create camera render data
	int window_width, window_height;
	glfwGetWindowSize(m_window, &window_width, &window_height);
	render_data->camera_render_data = m_camera->create_render_data(window_width, window_height);

	// Create grid render data
	render_data->grid_render_data = grid_manager->grid->create_render_data();

	return *render_data;
}

//--------------------------------------------------------------------------------
Timer::Timer() :
	dt(0.0f),
m_last_frame_time(0.0f)
{

}

//--------------------------------------------------------------------------------
void Timer::update() {
	ZoneScoped;
	float m_current_frame_time = (float) glfwGetTime();
	dt = m_current_frame_time - m_last_frame_time;
	m_last_frame_time = m_current_frame_time;
}

Grid_Manager::Grid_Manager()
: grid_execution_state({})
{
	ZoneScoped;

	create_new_grid();
};

//--------------------------------------------------------------------------------
void Grid_Manager::create_new_grid() {
	ZoneScoped;
	
	grid_execution_state = {};
	grid = new Grid();
}

void Grid_Manager::update(double dt, Grid_UI_Controls_Info ui_info) {
	switch (ui_info.button_type) {
		case GRID_NO_BUTTON_PRESSED:
			break;
		case GRID_RESET_BUTTON_PRESSED:
			create_new_grid();
			return;
			// if you remove the return somehow later, dont forget a break statement here :)
			// break;
		case GRID_NEXT_ITERATION_BUTTON_PRESSED:
			// only set run_manual_next_iteration if we are stopped
			if (!grid_execution_state.is_running) {
				grid_execution_state.run_manual_next_iteration = true;
			}
			break;
		case GRID_START_STOP_BUTTON_PRESSED:
			grid_execution_state.is_running = !grid_execution_state.is_running;
			grid_execution_state.time_since_last_iteration = 0.0f;
			break;
		default:
			break;
	}
	grid_execution_state.grid_speed = ui_info.grid_speed_slider_value;

	if (grid_execution_state.is_running) {
		//assert(grid_execution_state.grid_speed > 0.0f);
		grid_execution_state.time_since_last_iteration += (float) dt;
		float threshold = 1.0f / grid_execution_state.grid_speed;
		if (grid_execution_state.time_since_last_iteration >= threshold) {
			grid->next_iteration();
			grid_execution_state.time_since_last_iteration = 0.0f;
		}
	} else {
		if (grid_execution_state.run_manual_next_iteration) {
			grid_execution_state.run_manual_next_iteration = false;
			grid->next_iteration();
		}
	}
}