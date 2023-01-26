#pragma once
//--------------------------------------------------------------------------------
#include "opengl.hpp"

#include <vector>

//--------------------------------------------------------------------------------
class Cube_Render_Data {
public:
	Cube_Render_Data();

	void send_data_to_vertexbuffer(GLuint VAO, GLuint VBO);
	//--------------------------------------------------------------------------------
	glm::vec3 position;

	glm::mat4 m_model;
	std::vector<float> vertices;
};

//--------------------------------------------------------------------------------
class Cube {
public:
	Cube();

	void draw();

	void register_backbuffer_and_attributes(GLuint VAO, GLuint VBO);

	Cube_Render_Data* create_render_data();

	void update_model_matrix();
	//--------------------------------------------------------------------------------
	// data
	glm::vec3 m_position;
	glm::vec3 m_normal;
	float m_angle;

	GLuint m_VAO;
	GLuint m_VBO;

	glm::mat4 m_model;
};