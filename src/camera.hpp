#pragma once

//--------------------------------------------------------------------------------
#include "Tracy.hpp"

#include "opengl.hpp"

//--------------------------------------------------------------------------------
enum class Camera_Move_Direction {
	UNDEFINED,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//--------------------------------------------------------------------------------
class Camera {
public:
	Camera();

	void initialise();

	void move(Camera_Move_Direction);

	glm::mat4 get_view_matrix();

	glm::mat4 get_projection_matrix(int viewport_width, int viewport_height);
	//--------------------------------------------------------------------------------
	glm::vec3 position;
	glm::mat3 orientation_vector_matrix;
	float m_speed;
};

//--------------------------------------------------------------------------------
class Mouse {
public:
	Mouse();

	void initialise(float x, float y);

	void update();
 
	//--------------------------------------------------------------------------------
	float m_x;
	float m_y;
};
