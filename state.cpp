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
void State::update(std::vector<UI_Event_Type> event_queue) {
	ZoneScoped;
	m_timer->update();
	grid_manager->update(event_queue);
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

Grid_Manager::Grid_Manager()
: grid_execution_state({})
{
	ZoneScoped;

	create_new_grid();
};

//--------------------------------------------------------------------------------
void Grid_Manager::create_new_grid() {
	ZoneScoped;

	grid_execution_state.is_running = false;
	grid_execution_state.run_manual_next_iteration = false;
	grid = new Grid();
}

void Grid_Manager::update(std::vector<UI_Event_Type> event_queue) {
	// @Cleanup: This design pattern might be really bad, ie we process the whole queue in this update call. (Set a max number of events like 5 or something?). What if we somehow spam the queue and we therefore block everything with these events. Might make it really irresponsive. This is not a problem yet and it should be easy to fix later so we dont do anything yet. 

	// DONT FORGET TO CLEAR THE QUEUE AFTER THIS LOOP
	for (UI_Event_Type event: event_queue) {
		switch (event) {
			// reset no matter what, we always reset the grid if we get that event. This cant be overwritten by other events later in the queue.
			case GRID_RESET:
				create_new_grid();
				event_queue.clear();
				break;
			// all other events can be overwritten by subsequent events in the queue. This doesnt happen in practice, unless we are somehow unresponsive but the ui caches all user inputs. If that happens we just take the last action. So to summarize, we only process the last event, except for grid resets, which always get executed.
			case GRID_MANUAL_NEXT_ITERATION:
				// only set run_manual_next_iteration if we are stopped
				if (!grid_execution_state.is_running) {
					grid_execution_state.run_manual_next_iteration = true;
				}
				break;
			case GRID_START_RUNNING:
				grid_execution_state.is_running = true;
				break;
			case GRID_STOP_RUNNING:
				grid_execution_state.is_running = false;
				break;
			default:
				break;
		}
	}
	// DONT FORGET TO CLEAR THE QUEUE!
	event_queue.clear(); 

	if (grid_execution_state.is_running) {
		grid->next_iteration();
	} else {
		if (grid_execution_state.run_manual_next_iteration) {
			grid_execution_state.run_manual_next_iteration = false;
			grid->next_iteration();
		}
	}

}