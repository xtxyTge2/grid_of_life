#include "renderer.hpp"

//--------------------------------------------------------------------------------
Renderer::Renderer() :
	m_window(nullptr),
grid_cubes_VAO(0),
grid_cubes_VBO(0),
cubes_instances_VBO(0),
light_VAO(0),
texture_catalog(nullptr),
cubes_shader_program(nullptr)
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

	glGenVertexArrays(1, &grid_cubes_VAO);
	glGenBuffers(1, &grid_cubes_VBO);
	glGenBuffers(1, &cubes_instances_VBO);
	glGenVertexArrays(1, &light_VAO);

	cubes_shader_program = std::make_unique<Shader_Program>(cubes_vertex_shader_path, cubes_fragment_shader_path);
	cubes_shader_program->link_and_cleanup();

	//--------------------------------------------------------------------------------
	std::vector<std::string> texture_file_paths = {
		"assets/textures/container.jpg"
	};

	texture_catalog = std::make_unique<Texture_Catalog>();
	texture_catalog->load_and_bind_all_textures(texture_file_paths);

	cubes_shader_program->load_texture_catalog(*texture_catalog);

	//light_shader_program = std::make_unique<Shader_Program>(light_vertex_shader_path, light_fragment_shader_path);
	//light_shader_program->link_and_cleanup();

	initialise_cube_rendering();
}

	
//--------------------------------------------------------------------------------
void Renderer::update_shader_program(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
	ZoneScoped;
	cubes_shader_program->set_uniform_mat4("model", model);
	cubes_shader_program->set_uniform_mat4("view", view);
	cubes_shader_program->set_uniform_mat4("projection", projection);
}

void Renderer::render_frame(std::shared_ptr < World > world, std::shared_ptr < Cube_System > cube_system) {
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

	
	glBindBuffer(GL_ARRAY_BUFFER, grid_cubes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(grid_cubes_VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,  5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// define buffer which holds the 4x4 mvp matrices of each rendered grid cube. Have to send each individual matrix row as a vec4 of floats.
	glBindBuffer(GL_ARRAY_BUFFER, cubes_instances_VBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*
	glBindVertexArray(light_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubes_instances_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	*/
}


void Renderer::render_grid(std::shared_ptr < Cube_System > cube_system) {
	ZoneScoped;


	glBindVertexArray(grid_cubes_VAO);
	
	cubes_shader_program->use();

	// send the mvp matrices to the corresponding buffer
	std::size_t number_of_cubes = cube_system->number_of_translation_data;
	std::size_t data_size = sizeof(glm::vec3) * number_of_cubes;
	const void * data = reinterpret_cast<const void *>(cube_system->cubes_translation_data.data());
	glBindBuffer(GL_ARRAY_BUFFER, cubes_instances_VBO);
	glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STREAM_DRAW);

	// draw all cubes.
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<GLsizei>(number_of_cubes));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
}

void Renderer::set_projection_view_matrix_in_shader(std::shared_ptr < World > world) {
	int window_width, window_height;
	glfwGetWindowSize(world->m_window, &window_width, &window_height);
	glm::mat4 view_matrix = world->m_camera->get_view_matrix();
	glm::mat4 projection_matrix = world->m_camera->get_projection_matrix(window_width, window_height);
	glm::mat4 projection_view_matrix = projection_matrix * view_matrix;
	
	cubes_shader_program->use();
	cubes_shader_program->set_uniform_mat4("projection_view", projection_view_matrix);

	glm::vec3 light_color = glm::vec3(1.0f);
	glm::vec3 object_color = glm::vec3(0.3f, 0.3f, 1.0f);
	cubes_shader_program->set_uniform_vec3("light_color", light_color);
	cubes_shader_program->set_uniform_vec3("object_color", object_color);

	/*
	light_shader_program->use();
	unsigned int projection_light_shader_location = glGetUniformLocation(light_shader_program->id, "projection_view");
	glUniformMatrix4fv(projection_light_shader_location, 1, GL_FALSE, glm::value_ptr(projection_view_matrix));
	*/
	//fragment_color = texture(texture1, texture_coordinate);
}


//--------------------------------------------------------------------------------
void Renderer::render_world(std::shared_ptr < World > world, std::shared_ptr < Cube_System > cube_system) {
	ZoneScoped;
	

	glClearColor(0.0f, 25.0f / 255.0f, 51.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	set_projection_view_matrix_in_shader(world);
	render_grid(cube_system);

	/*
	light_shader_program->use();
	unsigned int light_shader_object_color_location = glGetUniformLocation(light_shader_program->id, "object_color");
	glUniformMatrix3fv(light_shader_object_color_location, 1, GL_FALSE, glm::value_ptr(light_color));

	unsigned int light_shader_color_location = glGetUniformLocation(light_shader_program->id, "light_color");
	glUniformMatrix3fv(light_shader_color_location, 1, GL_FALSE, glm::value_ptr(object_color));

	glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, -3.0f);
	glm::mat4 light_model = glm::mat4(1.0f);
	light_model = glm::translate(light_model, light_pos);
	light_model = glm::scale(light_model, glm::vec3(0.2f)); 

	glBindVertexArray(light_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);	
	*/
}