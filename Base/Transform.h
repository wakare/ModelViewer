#pragma once
#include "WJMath.h"

class Transform
{
private:
	Matrix4 m_transformMatrix;

public:
	Transform();
	
	const Transform& operator*= (const Transform& otherTransform);
	const Transform operator* (const Transform& otherTransform) const;

	void	SetTransformMatrix(Matrix4 matrix);
	void	SetTransformMatrix(float matrix[16]);
	bool	SetProjectionTransform(float fov, float aspect, float nearZ, float farZ);
	void	Rotate(float fRotateAngle, Vector3 Axis);
	void	Translate(float fX, float fY, float fZ);
	void	Scale(float fX, float fY, float fZ);

	Matrix4 GetTransformMatrix() const;

	static void GetSelfRotateInfo(double deltaX, double deltaY, float rotateSpeed, Vector3& outRotateAxis, float& outfRotateAngle);
};