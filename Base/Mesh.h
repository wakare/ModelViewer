#pragma once
#ifndef MESH_H
#define MESH_H

#include <vector>
#include "ISceneObject.h"
#include "I3DObject.h"
#include "TrianglePrimitive.h"
#include "ShaderMgr.h"

class Mesh: public ISceneObject
{
private:
	float* m_pTriangleArrayData;
	GLuint m_VAO;
	GLuint m_VBO;
	
	void _moveToScreenCenter();
	bool InitShaderMgr();
public:
	unsigned m_nTriangleNumber;
	Vector3 m_defaultColor;

	Mesh();
	~Mesh();

	bool InitFromSTLData(char* pTriangleData, int nTriangleSize);
	PrimitiveData GetPrimitiveData();
	bool InitSpaceAccelerateStruct();
	void Render(Transform viewTransform, Transform projTransform);
	bool InitGLData();
	void Update();
};
#endif // MESH_H