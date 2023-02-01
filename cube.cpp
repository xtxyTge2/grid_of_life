#pragma once

//--------------------------------------------------------------------------------
#include "cube.hpp"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
Cube::Cube() :
	m_position(glm::vec3(0.0f, 0.0f, 0.0f)),
	m_angle(0.0f)
{
	
}

glm::mat4 Cube::get_model_matrix() {
	ZoneScoped;
	glm::mat4 model = glm::mat4(1.0f);

	if (true) {
		model = glm::translate(model, m_position);
		model = glm::rotate(model, glm::radians(m_angle), glm::vec3(1.0f, 0.3f, 0.5f));
	} else {
		model = glm::rotate(model, glm::radians(m_angle), glm::vec3(1.0f, 0.3f, 0.5f));
		model = glm::translate(model, m_position);
	}
	

	return model;
}
