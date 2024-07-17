
#ifndef _RENDER_ABSTRACT_CAMERA_H_
#define _RENDER_ABSTRACT_CAMERA_H_

#include "glm/glm.hpp"


const uint32_t KEY_W = 0x57;
const uint32_t KEY_A = 0x41;
const uint32_t KEY_S = 0x53;
const uint32_t KEY_D = 0x44;

const int32_t NO_MOVE = 0;
const int32_t MOVE_LEFT = 0x001;
const int32_t MOVE_RIGHT = 0x002;
const int32_t MOVE_FRONT = 0x004;
const int32_t MOVE_BACK = 0x008;

class AbstractCamera
{
public:
	AbstractCamera() = default;
	virtual ~AbstractCamera() = default;

public:
    virtual void OnLeftMouseDown(int x, int y) {}
	virtual void OnLeftMouseUp(int x, int y) {}
    virtual void OnMiddleMouseDown(int x, int y) {}
	virtual void OnMiddleMouseUp(int x, int y) {}
	virtual void OnRightMouseDown(int x, int y) {}
	virtual void OnRightMouseUp(int x, int y) {}
	virtual void OnMouseMove(int x, int y) {}
	virtual void OnKeyDown(uint32_t key) {}
	virtual void OnKeyUp(uint32_t key) {}

	virtual void Update() = 0;

	glm::mat4 GetViewMatrix() const { return view_matrix_; }
	glm::mat4 GetProjectionMatrix() const { return projection_matrix_; }

protected:
	glm::mat4 view_matrix_;
	glm::mat4 projection_matrix_;

	//摄像机所在位置
	glm::vec3	position_ { 0.0f, 0.0f, 0.0f };
	//摄像机观察方向
	glm::vec3	front_ { 0.0f, 0.0f, -1.0f };
	//摄像机穹顶方向
	glm::vec3	top_ { 0.0f, 1.0f, 0.0f };
	//摄像机移动速度
	float		move_speed_factor_ { 0.1f };
	//鼠标移动灵敏度
	float		mouse_sensitivity_factor_ { 0.1f };
	//移动状态（前后左右、不移动）
	int32_t	move_state_ { 0 };
	//是否鼠标正在按下移动中
	bool		is_mouse_moving_ { false };
	//上次鼠标所在的屏幕空间位置
	int			last_mouse_x_ { 0 };
	int			last_mouse_y_ { 0 };
};

#endif