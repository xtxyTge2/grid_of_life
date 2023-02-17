#include "world.hpp"

//--------------------------------------------------------------------------------
World::World() :
	m_camera(nullptr),
m_mouse(nullptr),
m_window(nullptr),
grid_manager(nullptr)
{

}

//--------------------------------------------------------------------------------
void World::initialise(GLFWwindow* window) {
	ZoneScoped;
	m_window = window;

	m_mouse = std::make_unique < Mouse > ();
	m_mouse->initialise(0.0f, 0.0f);

	m_camera = std::make_unique < Camera > ();
	m_camera->position = glm::vec3(Chunk::rows / 2, Chunk::columns / 2, 1000.0f);

	// left/right/x-axis direction vector
	m_camera->orientation_vector_matrix[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	
	// up/down/y-axis direction vector
	m_camera->orientation_vector_matrix[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	
	// front/back/z-axis direction vector
	m_camera->orientation_vector_matrix[2] = glm::vec3(0.0f, 0.0f, -1.0f);
	
	m_camera->m_speed = 50.0f;
	
	grid_manager = std::make_shared<Grid_Manager>();
}


//--------------------------------------------------------------------------------
void World::update(double dt, const Grid_UI_Controls_Info& grid_ui_controls_info) {
	ZoneScoped;
	grid_manager->update(dt, grid_ui_controls_info);

	process_input(dt);
}

//--------------------------------------------------------------------------------
void World::process_input(double dt) {
	ZoneScoped;
	m_camera->m_speed = 50.0f * (float) dt;

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_window, true);
	}
	
	Camera_Move_Direction direction = Camera_Move_Direction::UNDEFINED;
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