#pragma once
#include <memory>
#include "ISceneObject.h"

class Point : public ISceneObject
{
private:
	Vector3 m_position;
	Vector3 m_color;
	GLuint m_VAO;
	GLuint m_VBO;

public:
	Point(Vector3 position, Vector3 color): m_color(color), m_position(position) {};

	PrimitiveData GetPrimitiveData();
	bool InitSpaceAccelerateStruct();
	void Update();
	void SetPosition(const Vector3& position);
	void SetColor(const Vector3& color);
};