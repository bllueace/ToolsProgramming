#pragma once
#include <DirectXMath.h>
#include "d3d11.h"
#include <SimpleMath.h>
using namespace DirectX;
using namespace SimpleMath;
class Camera
{
public:
	float speed = 10;

	Camera();


	Vector3 Rotation = Vector3(0, 0, 0);
	Vector3 Position = Vector3(0, 0, 6);
	Vector3 forward = Vector3(0, 0, 0);
	Vector3 up = Vector3(0, 0, 0);
	Vector3 right = Vector3(0, 0, 0);
	// Update function receives delta time from parent (used for frame independent updating).
	void update();
	//Position
	Vector3 getPosition();

	void setPos(Vector3 p);
	void setRot(Vector3 r);
	//forward
	void setForward(Vector3 f);
	//Look at
	Vector3 getLookAt();
	//up
	Vector3 getUp();
	void setUp(Vector3 u);


	//Movement for cam
	void StrafeRight();
	void StrafeLeft();
	void moveForward();
	void moveBack();
	void moveUp();
	void moveDown();
	void Camera::mouseRot(float mouseX, float mouseY);
	//rotation
	void rotRight();
	void rotLeft();
	void rotUp();
	void rotDown();
	float rSpeed = 1.0f;

	float moveSpeed = 0.02;

	void focusPos();

	XMMATRIX trans;

	void UpdateViewMatrix();

	XMMATRIX viewMatrix;
	~Camera();
};

