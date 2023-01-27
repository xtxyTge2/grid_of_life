#pragma once

//--------------------------------------------------------------------------------
#include "state.hpp"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
State::State() :
	grid(nullptr),
	m_camera(nullptr),
	m_mouse(nullptr),
	m_window(nullptr),
	m_timer(nullptr)
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
	m_camera->position = glm::vec3(10.0f, 10.0f, 100.0f);

	// left/right/x-axis direction vector
	m_camera->orientation_vector_matrix[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	
	// up/down/y-axis direction vector
	m_camera->orientation_vector_matrix[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	
	// front/back/z-axis direction vector
	m_camera->orientation_vector_matrix[2] = glm::vec3(0.0f, 0.0f, -1.0f);
	
	m_camera->m_speed = 50.0f;
	
	reset_grid();
}


//--------------------------------------------------------------------------------
void State::update() {
	ZoneScoped;
	m_timer->update();
}

//--------------------------------------------------------------------------------
void State::framebuffer_size_callback(int width, int height) {
	ZoneScoped;
	glViewport(0, 0, width, height);
}

//--------------------------------------------------------------------------------
void State::process_input() {
	ZoneScoped;
	m_camera->m_speed = 50.0f * m_timer->m_delta_time;

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
void State::reset_grid() {
	ZoneScoped;
	int rows = 20;
	int columns = 20;
	
	int origin_row = 10;
	int origin_column = 10;

	grid = new Grid(rows, columns, origin_row, origin_column);

	grid->cells(origin_row, origin_column) = true;
	grid->cells(origin_row + 1, origin_column) = true;
	grid->cells(origin_row + 1, origin_column - 1) = true;
	grid->cells(origin_row + 1, origin_column - 2) = true;
	grid->cells(origin_row + 2, origin_column - 1) = true;

	grid->cells(origin_row, origin_column - 4) = true;
	grid->cells(origin_row, origin_column - 5) = true;
	grid->cells(origin_row, origin_column - 6) = true;
	grid->cells(origin_row + 1, origin_column - 5) = true;

	grid->update_neighbour_count();
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
	render_data->grid_render_data = grid->create_render_data();

	return *render_data;
}

//--------------------------------------------------------------------------------
Timer::Timer() :
	m_delta_time(0.0f),
m_last_frame_time(0.0f)
{

}

//--------------------------------------------------------------------------------
void Timer::update() {
	ZoneScoped;
	float m_current_frame_time = (float) glfwGetTime();
	m_delta_time = m_current_frame_time - m_last_frame_time;
	m_last_frame_time = m_current_frame_time;
}