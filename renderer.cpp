#include "renderer.hpp"

//--------------------------------------------------------------------------------
Renderer::Renderer() :
	m_window(nullptr),
m_VAO(0),
m_VBO(0),
cubes_instances_VBO(0),
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
	glGenBuffers(1, &cubes_instances_VBO);

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


	initialise_cube_rendering();
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

	swap_backbuffer();
}

void Renderer::swap_backbuffer() {
	ZoneScoped; 

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

	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,  5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// define buffer which holds the 4x4 mvp matrices of each rendered grid cube. Have to send each individual matrix row as a vec4 of floats.
	glBindBuffer(GL_ARRAY_BUFFER, cubes_instances_VBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) 0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) (1 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) (2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *) (3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}




void Renderer::render_grid(std::vector<glm::mat4>& cubes_model_data) {
	ZoneScoped;

	glBindVertexArray(m_VAO);
	m_shader_program->use();

	// send the mvp matrices to the corresponding buffer
	glBindBuffer(GL_ARRAY_BUFFER, cubes_instances_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * cubes_model_data.size(), (const void *) cubes_model_data.data(), GL_STREAM_DRAW);

	// draw all cubes.
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<GLsizei>(cubes_model_data.size()));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
}

void Renderer::set_projection_view_matrix_in_shader(std::shared_ptr<World> world) {
	int window_width, window_height;
	glfwGetWindowSize(world->m_window, &window_width, &window_height);
	glm::mat4 view_matrix = world->m_camera->get_view_matrix();
	glm::mat4 projection_matrix = world->m_camera->get_projection_matrix(window_width, window_height);
	glm::mat4 projection_view_matrix = projection_matrix * view_matrix;
	
	m_shader_program->use();
	unsigned int projection_view_matrix_location = glGetUniformLocation(m_shader_program->id, "projection_view");
	glUniformMatrix4fv(projection_view_matrix_location, 1, GL_FALSE, glm::value_ptr(projection_view_matrix));
}

//--------------------------------------------------------------------------------
void Renderer::render_world(std::shared_ptr<World> world, std::shared_ptr<Cube_System> cube_system) {
	ZoneScoped;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	set_projection_view_matrix_in_shader(world);

	render_grid(cube_system->cubes_model_data);
}