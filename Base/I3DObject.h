#pragma once
#include <vector>

#include "WJMath.h"
#include "Ray.h"

class I3DObject
{
public:
	bool m_bSelected = false;

	virtual Vector3 GetVertex3DCoordByIndex(int nIndex) = 0;
	virtual Vector3 GetNormal() = 0;
	virtual Vector3 GetUV(int nIndex) = 0;
	virtual int GetVertexCount() = 0;

	virtual float HasIntersectionAlongRay(const Ray& ray, double& outU, double& outV, double& outW) = 0;
	void SetSelected(bool bSelected) { m_bSelected = bSelected; }
};