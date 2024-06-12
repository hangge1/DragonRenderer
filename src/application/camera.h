
#ifndef _APPLICATION_CAMERA_H_
#define _APPLICATION_CAMERA_H_

#include "glm/glm.hpp"

#define NO_MOVE 0
#define MOVE_LEFT 0x001
#define MOVE_RIGHT 0x002
#define MOVE_FRONT 0x004
#define MOVE_BACK 0x008

#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44

class Camera
{
public:
	Camera(float fovy, float aspect, float n, float f, const glm::vec3& top);

	~Camera();

public:
	void onRMouseDown(int x, int y);

	void onRMouseUp(int x, int y);

	void onMouseMove(int x, int y);

	void onKeyDown(uint32_t key);

	void onKeyUp(uint32_t key);

	void update();

	glm::mat4 getViewMatrix()const { return view_matrix_; }

	glm::mat4 getProjectionMatrix()const { return projection_matrix_; }

private:
	void pitch(int yoffset);

	void yaw(int xoffset);

private:
	glm::mat4 view_matrix_;
	glm::mat4 projection_matrix_;

	glm::vec3	position_ { 0.0f,0.0f,1.0f };
	glm::vec3	front_ { 0.0f,0.0f,-1.0f };
	glm::vec3	top_ { 0.0f,1.0f,0.0f };
	float		move_speed_ { 0.01f };

	float		pitch_ { 0.0f };
	float		yaw_ { -90.0f };
	float		mouse_sensitivity_ { 0.1f };

	uint32_t	move_state_ { NO_MOVE };
	bool		is_mouse_moving_ { false };
	int			current_mouse_x_ { 0 };
	int			current_mouse_y_ { 0 };
};

#endif