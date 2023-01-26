#pragma once

//--------------------------------------------------------------------------------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

//--------------------------------------------------------------------------------
enum Camera_Move_Direction {
	UNDEFINED,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera_Render_Data {
public:
	Camera_Render_Data();
	//--------------------------------------------------------------------------------
	// data
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

//--------------------------------------------------------------------------------
class Camera {
public:
	Camera();

	void initialise();

	void move(Camera_Move_Direction);

	glm::mat4 get_view_matrix();

	glm::mat4 get_projection_matrix(int viewport_width, int viewport_height);

	Camera_Render_Data* create_render_data(int viewport_width, int viewport_height);
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
