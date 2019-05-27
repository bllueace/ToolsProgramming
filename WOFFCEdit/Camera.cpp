#include "Camera.h"
Camera::Camera()
{
// 	Position.set(0, 0, 6);
// 	Rotation.set(0, 0, 0);
	Position = Vector3(20, 10, 0);
	Rotation = Vector3(-45, -100.0, 0.0);
	
	update();

}
Camera::~Camera()
{
}
void Camera::update()
{
	float cosR, cosP, cosY; //temp values for sin/cos from
	float sinR, sinP, sinY;

	// handle rotation
	cosY = cosf(Rotation.y *3.1415 / 180);
	cosP = cosf(Rotation.x *3.1415 / 180);
	cosR = cosf(Rotation.z *3.1415 / 180);
	sinY = sinf(Rotation.y *3.1415 / 180);
	sinP = sinf(Rotation.x *3.1415 / 180);
	sinR = sinf(Rotation.z *3.1415 / 180);

	forward.x = sinY * cosP;
	forward.y = sinP;
	forward.z = cosP * -cosY;

	// Up Vector
	up.x = -cosY * sinR - sinY * sinP * cosR;
	up.y = cosP * cosR;
	up.z = -sinY * sinR - sinP * cosR * -cosY;

	// Side Vector (right)
	right = forward.Cross(up);

	//only rotate to a certain point
	Rotation.x = min(Rotation.x, +89.f);
	Rotation.x = max(Rotation.x, -89.);
}


Vector3 Camera::getPosition()
{
	return Position;
}

Vector3 Camera::getLookAt()
{
	return Position + forward;
}

Vector3 Camera::getUp()
{
	return up;
}
void Camera::setPos(Vector3 p)
{
	Position = p;
}

void Camera::setRot(Vector3 r)
{
	Rotation = r;
}

void Camera::setForward(Vector3 f)
{
	forward = f;
}
void Camera::setUp(Vector3 u)
{
	up = u;
}

void Camera::StrafeRight()
{
	Vector3 temp = forward.Cross(up);
	Position.x += temp.x;
	Position.y += temp.y;
	Position.z += temp.z;
}

void Camera::StrafeLeft()
{
	Vector3 temp = forward.Cross(up);
	Position.x -= temp.x;
	Position.y -= temp.y;
	Position.z -= temp.z;

}
void Camera::moveForward()
{
	Position.x += forward.x;
	Position.y += forward.y;
	Position.z += forward.z;
}

void Camera::moveBack()
{
	Position.x -= forward.x;
	Position.y -= forward.y;
	Position.z -= forward.z;
}

void Camera::moveUp()
{
	Position.x += up.x;
	Position.y += up.y;
	Position.z += up.z;
}


void Camera::moveDown()
{
	Position.x -= up.x;
	Position.y -= up.y;
	Position.z -= up.z;
}

void Camera::rotRight()
{
	Rotation.y += speed;
}
void Camera::rotLeft()
{

	Rotation.y -= speed;
}

void Camera::rotUp()
{
	Rotation.x += speed;
}

void Camera::rotDown()
{
	Rotation.x -= speed;
}

void Camera::focusPos()
{

}

void Camera::mouseRot(float mouseX, float mouseY)
{
	Rotation.x -= mouseY * rSpeed;

	Rotation.y += mouseX * rSpeed;
}

//void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
//{
//	//Calculate camera rotation matrix
//	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
//	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
//	XMVECTOR camTarget = XMVector3TransformCoord(this->forward, camRotationMatrix);
//	//Adjust cam target to be offset by the camera's current position
//	camTarget += this->Position;
//	//Calculate up direction based on current rotation
//	XMVECTOR upDir = XMVector3TransformCoord(this->up, camRotationMatrix);
//	//Rebuild view matrix
//	this->viewMatrix = XMMatrixLookAtLH(this->Position, camTarget, upDir);
//}