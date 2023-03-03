#pragma once

//--------------------------------------------------------------------------------
#include <Tracy/Tracy.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

//--------------------------------------------------------------------------------
enum class Camera_Move_Direction {
	NO_DIRECTION,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	ROTATE_AROUND_Y_AXIS
};

//--------------------------------------------------------------------------------
class Camera {
public:
	Camera();

	void update(double dt);

	void initialise();

	void add_offset_and_clip_fov(float yoffset);

	void move_in_current_direction(double dt);

	void rotate_around_y_axis(double dt);

	glm::mat4 get_view_matrix();

	glm::mat4 get_projection_matrix(int viewport_width, int viewport_height);
	//--------------------------------------------------------------------------------
	glm::vec3 position;
	glm::vec3 target_position;

	glm::vec3 left_direction_vector;
	glm::vec3 up_direction_vector;
	glm::vec3 front_direction_vector;

	float fov;
	Camera_Move_Direction direction;
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
