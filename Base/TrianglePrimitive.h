#pragma once
#include "I3DObject.h"
#include <vector>
#include <memory>

struct TrianglePrimitive : public I3DObject
{
	const int g_nVertexCount = 3;

	float m_normalVec[3];
	float m_vertices[3][3];
	float m_UVs[3][2];

	int GetVertexCount();
	Vector3 GetVertex3DCoordByIndex(int nIndex) { return m_vertices[nIndex]; }
	Vector3 GetNormal() { return m_normalVec; };
	Vector3 GetUV(int nIndex) { return { m_UVs[nIndex][0], m_UVs[nIndex][1], 0.0f}; };
	float HasIntersectionAlongRay(const Ray& ray, double& outU, double& outV, double& outW);
};
