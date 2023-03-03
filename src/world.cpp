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
	m_camera->position = glm::vec3(Chunk::rows / 2, Chunk::columns / 2, 1250.0f);

	m_camera->target_position = glm::vec3(0.0f);

	grid_manager = std::make_shared<Grid_Manager>();
}


//--------------------------------------------------------------------------------
void World::update(double dt, const Grid_UI_Controls_Info& grid_ui_controls_info) {
	ZoneScoped;

	grid_manager->update(dt, grid_ui_controls_info);

	process_input(dt);

	m_camera->update(dt);
}

//--------------------------------------------------------------------------------
void World::process_input(double dt) {
	ZoneScoped;
	

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_window, true);
	}
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
		m_camera->direction = Camera_Move_Direction::FORWARD;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		m_camera->direction = Camera_Move_Direction::BACKWARD;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
		m_camera->direction = Camera_Move_Direction::LEFT;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		m_camera->direction = Camera_Move_Direction::RIGHT;
	}
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS) {
		m_camera->direction = Camera_Move_Direction::ROTATE_AROUND_Y_AXIS;
	}
	if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS) {
		m_camera->direction = Camera_Move_Direction::NO_DIRECTION;
	}
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		// reset camera with spacebar
		m_camera->direction = Camera_Move_Direction::NO_DIRECTION;
		m_camera->position = glm::vec3(Chunk::rows / 2, Chunk::columns / 2, 1250.0f);
		m_camera->target_position = glm::vec3(0.0f);
	}
}