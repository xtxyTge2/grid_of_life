#pragma once

//--------------------------------------------------------------------------------
#include "renderer.hpp"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
World_Renderer::World_Renderer() :
	m_window(nullptr),
	m_VAO(0),
	m_VBO(0),
	texture_catalog(nullptr),
	m_shader_program(nullptr)
{

}

//--------------------------------------------------------------------------------
void World_Renderer::initialise(GLFWwindow* window) {
	ZoneScoped;
	m_window = window;
	
	int window_width, window_height;
	glfwGetWindowSize(m_window, &window_width, &window_height);

	glViewport(0, 0, (int) window_width, (int) window_height);	
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	m_shader_program = std::make_unique<Shader_Program>(m_vertex_shader_path, m_fragment_shader_path);
	m_shader_program->link_and_cleanup();

	//--------------------------------------------------------------------------------
	std::vector<std::string> texture_file_paths = {
		"data/textures/awesomeface.png",
		"data/textures/container.jpg"
	};

	texture_catalog = std::make_unique<Texture_Catalog>();
	texture_catalog->load_and_bind_all_textures(texture_file_paths);

	m_shader_program->load_texture_catalog(*texture_catalog);
}

	
//--------------------------------------------------------------------------------
void World_Renderer::update_shader_program(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
	ZoneScoped;
	m_shader_program->set_uniform_mat4("model", model);
	m_shader_program->set_uniform_mat4("view", view);
	m_shader_program->set_uniform_mat4("projection", projection);
}

//--------------------------------------------------------------------------------
void World_Renderer::render_frame(State_Render_Data& render_data) {
	ZoneScoped;
	Camera_Render_Data* camera_data = render_data.camera_render_data;
	update_shader_program(camera_data->model, camera_data->view, camera_data->projection);
	
	for (Cube_Render_Data& cube_data: render_data.grid_render_data->cubes_render_data) {
		cube_data.send_data_to_vertexbuffer(m_VAO, m_VBO);
	
		m_shader_program->set_uniform_mat4("model", cube_data.m_model);

		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}

//--------------------------------------------------------------------------------
void World_Renderer::render_cube(Cube_Render_Data& data) {

}

//--------------------------------------------------------------------------------
Renderer::Renderer() :
	window(nullptr),
	world_renderer(nullptr),
	ui_renderer(nullptr)
{

}

//--------------------------------------------------------------------------------
void Renderer::initialise(GLFWwindow* w) {
	ZoneScoped;
	window = w;

	world_renderer = std::make_unique<World_Renderer>();
	world_renderer->initialise(window);

	ui_renderer = std::make_unique<UI_Renderer>();
	ui_renderer->initialise(window);
}

//--------------------------------------------------------------------------------
void Renderer::render_frame(State_Render_Data& state_render_data) {
	ZoneScoped;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// world 
	world_renderer->render_frame(state_render_data);

	// ui
	ui_renderer->render_frame(state_render_data);

	glfwSwapBuffers(window);
}