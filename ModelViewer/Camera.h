#pragma once
#include "Transform.h"
#include "WJMath.h"

class Camera
{
private:
	Transform m_viewTransform;
	Vector3 m_cameraPosition;
	Vector3 m_lookDirection;
	Vector3 m_upDirection;
	Vector3 m_rightDirection;

	void _setViewTransform(Vector3 look, Vector3 up, Vector3 right);
public:
	float m_fMoveSpeed;
	float m_fRotateSpeed;

	Camera();
	Camera(Vector3 cameraPosition);

	void SetCameraPosition(Vector3 cameraPosition);
	void LookAtTarget(Vector3 targetPosition, Vector3 up);
	void SelfMove(Vector3 translate);
	void SelfRotate(double deltaX, double deltaY);

	const Vector3& GetCameraPosition() const;
	const Transform& GetViewTransform() const;
};