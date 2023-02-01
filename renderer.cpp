#pragma once

//--------------------------------------------------------------------------------
#include "renderer.hpp"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
Renderer::Renderer() :
	m_window(nullptr),
m_VAO(0),
m_VBO(0),
texture_catalog(nullptr),
m_shader_program(nullptr)
{

}

//--------------------------------------------------------------------------------
void Renderer::initialise(GLFWwindow* window) {
	ZoneScoped;
	m_window = window;
	
	int window_width, window_height;
	glfwGetWindowSize(m_window, &window_width, &window_height);

	glViewport(0, 0, (int) window_width, (int) window_height);
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	m_shader_program = std::make_unique < Shader_Program > (m_vertex_shader_path, m_fragment_shader_path);
	m_shader_program->link_and_cleanup();

	//--------------------------------------------------------------------------------
	std::vector<std::string> texture_file_paths = {
		"data/textures/awesomeface.png",
		"data/textures/container.jpg"
	};

	texture_catalog = std::make_unique < Texture_Catalog > ();
	texture_catalog->load_and_bind_all_textures(texture_file_paths);

	m_shader_program->load_texture_catalog(*texture_catalog);
}

	
//--------------------------------------------------------------------------------
void Renderer::update_shader_program(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
	ZoneScoped;
	m_shader_program->set_uniform_mat4("model", model);
	m_shader_program->set_uniform_mat4("view", view);
	m_shader_program->set_uniform_mat4("projection", projection);
}

void Renderer::render_frame(std::shared_ptr<World> world, std::shared_ptr<Cube_System> cube_system) {
	ZoneScoped;

	// world
	render_world(world, cube_system);
	// ui
	render_ui();

	glfwSwapBuffers(m_window);
}

void Renderer::render_ui() {
	ZoneScoped;
	// Render imgui frame
	// The imgui frame gets started in the ui_state->update() call. This call HAS to happen before this!
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//--------------------------------------------------------------------------------
void Renderer::initialise_cube_rendering() {
	ZoneScoped;
	const float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,  5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//--------------------------------------------------------------------------------
void Renderer::render_cube_system(std::shared_ptr<Cube_System> cube_system) {
	ZoneScoped;
	initialise_cube_rendering();

	initialise_cube_rendering();
	glBindVertexArray(m_VAO);
	for (int i = 0; i < cube_system->current_number_of_cubes; i++) {
		Cube current_cube = cube_system->cubes_array[i];
		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, current_cube.m_position);
		model = glm::rotate(model, glm::radians(current_cube.m_angle), glm::vec3(1.0f, 0.3f, 0.5f));

		m_shader_program->set_uniform_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);
}

//--------------------------------------------------------------------------------
void Renderer::render_world(std::shared_ptr<World> world, std::shared_ptr<Cube_System> cube_system) {
	ZoneScoped;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int window_width, window_height;
	glfwGetWindowSize(world->m_window, &window_width, &window_height);
	Camera_Render_Data* camera_data = world->m_camera->create_render_data(window_width, window_height);

	update_shader_program(camera_data->model, camera_data->view, camera_data->projection);

	render_cube_system(cube_system);
}