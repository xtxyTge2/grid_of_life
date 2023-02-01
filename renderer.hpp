#pragma once

//--------------------------------------------------------------------------------
#include <vector>
#include <memory>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

#include "state_render_data.hpp"
#include "ui_state.hpp"


//--------------------------------------------------------------------------------
class Renderer {
public:
	Renderer();

	void swap_backbuffer();

	void render_grid_cubes(std::shared_ptr<Cube_System> cube_system);

	void render_border_cubes(std::shared_ptr<Cube_System> cube_system);

	void initialise(GLFWwindow* window);

	void initialise_cube_rendering();

	void render_cube_system(std::shared_ptr<Cube_System> cube_system);

	void update_shader_program(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

	void render_frame(std::shared_ptr<World> world, std::shared_ptr<Cube_System> cube_system);

	void render_world(std::shared_ptr<World> world, std::shared_ptr<Cube_System> cube_system);

	void render_ui();
	//--------------------------------------------------------------------------------
	// data
	GLFWwindow* m_window;

	GLuint m_VAO;
	GLuint m_VBO;

	std::unique_ptr<Texture_Catalog> texture_catalog;

	std::unique_ptr<Shader_Program> m_shader_program;
	const std::string m_vertex_shader_path = "shader.vs";
	const std::string m_fragment_shader_path = "shader.fs";
};