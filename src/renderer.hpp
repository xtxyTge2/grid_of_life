#pragma once

//--------------------------------------------------------------------------------
#include <Tracy/Tracy.hpp>
#include "opengl.hpp"

#include <vector>
#include <memory>
#include <unordered_map>

#include "shader.hpp"
#include "world.hpp"
#include "cube_system.hpp"
#include "ui_state.hpp"


//--------------------------------------------------------------------------------
class Renderer {
public:
	Renderer();

	void swap_backbuffer();

	void render_grid(std::shared_ptr<Cube_System> cube_system);

	void initialise(GLFWwindow* window);

	void initialise_cube_rendering();

	void set_projection_view_matrix_in_shader(std::shared_ptr<World> world);

	std::vector<glm::mat4> compute_cube_mvp_data(std::shared_ptr<Cube_System> cube_system, glm::mat4 projection_view_matrix);

	void update_shader_program(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

	void render_frame(std::shared_ptr<World> world, std::shared_ptr<Cube_System> cube_system);

	void render_world(std::shared_ptr<World> world, std::shared_ptr<Cube_System> cube_system);

	void render_ui();
	//--------------------------------------------------------------------------------
	// data
	GLFWwindow* m_window;

	GLuint m_VAO;
	GLuint m_VBO;

	GLuint cubes_instances_VBO;

	std::unique_ptr<Texture_Catalog> texture_catalog;

	std::unique_ptr<Shader_Program> m_shader_program;
	const std::string m_vertex_shader_path = "shaders/shader.vertex_shader";
	const std::string m_fragment_shader_path = "shaders/shader.fragment_shader";
};