#include "cube.hpp"

//--------------------------------------------------------------------------------
Cube::Cube() :
	m_position(glm::vec3(0.0f, 0.0f, 0.0f)),
	m_angle(0.0f)
{
	
}

const glm::mat4 Cube::compute_model_matrix_no_rotation() const {
	ZoneScoped;

	glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), m_position);

	return model_matrix;
}


const glm::mat4 Cube::compute_model_matrix_with_rotation() const {
	ZoneScoped;

	glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), m_position);
	model_matrix = glm::rotate(model_matrix, m_angle, glm::vec3(1.0f, 0.3f, 0.5f));

	return model_matrix;
}