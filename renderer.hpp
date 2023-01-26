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

#include "cube.hpp"
#include "camera.hpp"
#include "shader_program.hpp"
#include "ui_renderer.hpp"
#include "state.hpp"
#include "texture.hpp"

//--------------------------------------------------------------------------------
class World_Renderer {
public:
	World_Renderer();

	void initialise(GLFWwindow* window);

	void update_shader_program(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

	void render_frame(State_Render_Data& render_data);

	void render_cube(Cube_Render_Data& data);
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

//--------------------------------------------------------------------------------
class Renderer {
public:
	Renderer();

	void initialise(GLFWwindow* w);

	void render_frame(State_Render_Data& state_render_data);
	//--------------------------------------------------------------------------------
	// data
	GLFWwindow* window;
	std::unique_ptr<World_Renderer> world_renderer;
	std::unique_ptr<UI_Renderer> ui_renderer;
};