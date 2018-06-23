#pragma once

#include <math.h>

#define PI 3.1415926f
#define FLOAT_DEVIATION 1e-6f
#define FLOAT_INFINITE 1e20f

inline bool IsZero(float f)
{
	return fabs(f) < FLOAT_DEVIATION;
}

class Matrix4;
class Vector4;
class Vector3;

class Vector4
{
public:
	float fX;
	float fY;
	float fZ;
	float fW;

	Vector4() :fX(0.0f), fY(0.0f), fZ(0.0f), fW(0.0f) {}
	Vector4(float X, float Y, float Z, float W) :fX(X), fY(Y), fZ(Z), fW(W) {}
	Vector4(const float value[4]) :fX(value[0]), fY(value[1]), fZ(value[2]), fW(value[3]) {}
	Vector4(Vector3 vector3, float fW);

	Vector4&	operator*=(float);
	Vector4&	operator/=(float);
	Vector4&	operator=(const Vector4&);
	Vector4&	operator*=(const Matrix4&);
	Vector4&	operator+=(const Vector4&);

	Vector4		operator*(const Matrix4&);
	Vector4		operator*(float);
	Vector4		operator+(const Vector4&);
	Vector4		operator-(const Vector4&);
	Vector4		operator-();
	Vector4		Cross(const Vector4&);
	float		operator*(Vector4);
	void		DivideW(); // reset (x,y,z,w) = (x/w, y/w, z/w, 1.0f)
	float		length();
};

class Matrix4 //4*4 matrix
{
public:
	float m_fMatrixArray[4][4];

	Matrix4(float matrixArray[4][4]);
	Matrix4(float matrixArray[16]);
	Matrix4();

	Matrix4		operator*(const Matrix4&) const;
	Vector4		operator[](int) const;
	Vector4		operator*(const Vector4&) const;
	Matrix4&	operator*=(const Matrix4&);
	
	float*		ToFloatArray();
	Matrix4		Transposition() const;
};

class Vector3
{
public:
	float fX;
	float fY;
	float fZ;

	Vector3() :fX(0.0f), fY(0.0f), fZ(0.0f) {}
	Vector3(float X, float Y, float Z) :fX(X), fY(Y), fZ(Z) {}
	Vector3(float vector[3]) : fX(vector[0]), fY(vector[1]), fZ(vector[2]) {}

	Vector3		operator-();
	Vector3&	operator*=(float);
	Vector3&	operator/=(float);
	Vector3&	operator=(const Vector3&);
	Vector3&	operator=(const Vector4&);
	Vector3&	operator+=(const Vector3&);

	Vector3		operator*(float) const;
	Vector3		operator+(const Vector3&) const;
	Vector3		operator-(const Vector3&) const;
	float		operator*(const Vector3) const;
	float		operator[](int) const;

	void		Normalize();
	Vector3		Cross(const Vector3&);
	float		Length();
};