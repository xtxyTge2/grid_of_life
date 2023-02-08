#include "renderer.hpp"

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
void Renderer::render_cube_system(std::shared_ptr<Cube_System> cube_system, glm::mat4 projection_view_matrix) {
	ZoneScoped;

	render_grid_cubes(cube_system, projection_view_matrix);
	render_border_cubes(cube_system, projection_view_matrix);
}

void Renderer::render_grid_cubes(std::shared_ptr<Cube_System> cube_system, glm::mat4 projection_view_matrix) {
	ZoneScoped;

	glBindVertexArray(m_VAO);
	m_shader_program->use();
	unsigned int mvp_matrix_location = glGetUniformLocation(m_shader_program->id, "mvp");

	// grid cubes, all have angle 0.0f, ie do not need to be rotated
	for (int i = 0; i < cube_system->current_number_of_grid_cubes; i++) {
		Cube& current_cube = cube_system->grid_cubes[i];
	
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), current_cube.m_position);

		glm::mat4 mvp_matrix = projection_view_matrix * model_matrix;
		glUniformMatrix4fv(mvp_matrix_location, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);
}


void Renderer::render_border_cubes(std::shared_ptr<Cube_System> cube_system, glm::mat4 projection_view_matrix) {
	ZoneScoped;

	glBindVertexArray(m_VAO);
	m_shader_program->use();
	unsigned int mvp_matrix_location = glGetUniformLocation(m_shader_program->id, "mvp");

	// pre compute the same fixed rotation matrix for every border cube.
	glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(1.0f, 0.3f, 0.5f));
	for (int i = 0; i < cube_system->current_number_of_border_cubes; i++) {
		Cube& current_cube = cube_system->border_cubes[i];

		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), current_cube.m_position);
		model_matrix = model_matrix * rotation_matrix;

		glm::mat4 mvp_matrix = projection_view_matrix * model_matrix;
		glUniformMatrix4fv(mvp_matrix_location, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
		
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
	glm::mat4 view_matrix = world->m_camera->get_view_matrix();
	glm::mat4 projection_matrix = world->m_camera->get_projection_matrix(window_width, window_height);
	glm::mat4 projection_view_matrix = projection_matrix * view_matrix;

	render_cube_system(cube_system, projection_view_matrix);
}