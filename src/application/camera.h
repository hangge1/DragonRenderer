
#ifndef _APPLICATION_CAMERA_H_
#define _APPLICATION_CAMERA_H_

#include "glm/glm.hpp"



class Camera
{
public:
	Camera(float fovy, float aspect, float n, float f, const glm::vec3& top);
	~Camera() = default;

	Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

public:
	void OnRMouseDown(int x, int y);

	void OnRMouseUp(int x, int y);

	void OnMouseMove(int x, int y);

	void OnKeyDown(uint32_t key);

	void OnKeyUp(uint32_t key);

	void Update();

	glm::mat4 GetViewMatrix() const { return view_matrix_; }

	glm::mat4 GetProjectionMatrix() const { return projection_matrix_; }

private:
	void Pitch(int yoffset);

	void Yaw(int xoffset);

private:
	glm::mat4 view_matrix_;
	glm::mat4 projection_matrix_;

	//���������λ��
	glm::vec3	position_ { 0.0f,0.0f,1.0f };
	//������۲췽��
	glm::vec3	front_ { 0.0f,0.0f,-1.0f };
	//�����񷶥����
	glm::vec3	top_ { 0.0f,1.0f,0.0f };
	//������ƶ��ٶ�
	float		move_speed_ { 0.01f };
	//�����pitch�Ž�
	float		pitch_angle_ { 0.0f };
	//�����yaw�Ž�
	float		yaw_angle_ { -90.0f };
	//����ƶ�������
	float		mouse_sensitivity_ { 0.1f };
	//�ƶ�״̬��ǰ�����ҡ����ƶ���
	int32_t	move_state_ { 0 };
	//�Ƿ�������ڰ����ƶ���
	bool		is_mouse_moving_ { false };
	//�ϴ�������ڵ���Ļ�ռ�λ��
	int			last_mouse_x_ { 0 };
	int			last_mouse_y_ { 0 };
};

#endif