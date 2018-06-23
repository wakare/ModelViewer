#include "TrianglePrimitive.h"
#include "Logger.h"
#include <assert.h>

int TrianglePrimitive::GetVertexCount()
{
	return g_nVertexCount;
}

float TrianglePrimitive::HasIntersectionAlongRay(const Ray& ray, double& outU, double& outV, double& outW)
{
	// return -1.0f represent have on intersection point
	float fDistance = -1.0f;

	// Get triangle plane equation
	Vector3 edgeVec1 = { m_vertices[1][0] - m_vertices[0][0],  m_vertices[1][1] - m_vertices[0][1] , m_vertices[1][2] - m_vertices[0][2] };
	Vector3 edgeVec2 = { m_vertices[2][0] - m_vertices[1][0],  m_vertices[2][1] - m_vertices[1][1] , m_vertices[2][2] - m_vertices[1][2] };
	
	Vector3 normalVec = edgeVec1.Cross(edgeVec2);
	Vector3 planeVertex = { m_vertices[0][0], m_vertices[0][1], m_vertices[0][2] };

	// Check ray is parallel to triangle plane
	if (fabs(ray.m_direction * normalVec) < FLOAT_DEVIATION)
		return fDistance;

	/*
		Calculate intersection between ray and triangle plane
		plane equation: (P - planeVertex) * normalVec = 0.0f
		ray equation: P = ray.position + t * ray.direction

		===> t = normalVec * (planeVertex - ray.position) / normalVec * ray.Direction
	*/

	fDistance = normalVec * (planeVertex - ray.m_startPosition) / (normalVec * ray.m_direction);
	if (fDistance < 0.0f)
		return -1.0f;

	Vector3 intersectPosition = ray.m_startPosition + ray.m_direction * fDistance;

	// Calculate intersection's triangle Barycentric Coordination
	double D =
		static_cast<double>(m_vertices[0][0]) * static_cast<double>(m_vertices[1][1]) * static_cast<double>(m_vertices[2][2]) +
		static_cast<double>(m_vertices[1][0]) * static_cast<double>(m_vertices[2][1]) * static_cast<double>(m_vertices[0][2]) +
		static_cast<double>(m_vertices[2][0]) * static_cast<double>(m_vertices[0][1]) * static_cast<double>(m_vertices[1][2]) -
		static_cast<double>(m_vertices[2][0]) * static_cast<double>(m_vertices[1][1]) * static_cast<double>(m_vertices[0][2]) -
		static_cast<double>(m_vertices[1][0]) * static_cast<double>(m_vertices[0][1]) * static_cast<double>(m_vertices[2][2]) -
		static_cast<double>(m_vertices[0][0]) * static_cast<double>(m_vertices[2][1]) * static_cast<double>(m_vertices[1][2]);

	if (IsZeroFloat(D))
		return -1.0f;

	double rhD = 1.0 / D;

	double Du =
		intersectPosition[0] * static_cast<double>(m_vertices[1][1]) * static_cast<double>(m_vertices[2][2]) +
		static_cast<double>(m_vertices[1][0]) * static_cast<double>(m_vertices[2][1]) * intersectPosition[2] +
		static_cast<double>(m_vertices[2][0]) * intersectPosition[1] * static_cast<double>(m_vertices[1][2]) -
		static_cast<double>(m_vertices[2][0]) * static_cast<double>(m_vertices[1][1]) * intersectPosition[2] -
		static_cast<double>(m_vertices[1][0]) * intersectPosition[1] * static_cast<double>(m_vertices[2][2]) -
		intersectPosition[0] * static_cast<double>(m_vertices[2][1]) * static_cast<double>(m_vertices[1][2]);

	double Dv =
		static_cast<double>(m_vertices[0][0]) * intersectPosition[1] * static_cast<double>(m_vertices[2][2]) +
		intersectPosition[0] * static_cast<double>(m_vertices[2][1]) * static_cast<double>(m_vertices[0][2]) +
		static_cast<double>(m_vertices[2][0]) * static_cast<double>(m_vertices[0][1]) * intersectPosition[2] -
		static_cast<double>(m_vertices[2][0]) * intersectPosition[1] * static_cast<double>(m_vertices[0][2]) -
		intersectPosition[0] * static_cast<double>(m_vertices[0][1]) * static_cast<double>(m_vertices[2][2]) -
		static_cast<double>(m_vertices[0][0]) * static_cast<double>(m_vertices[2][1]) * intersectPosition[2];

	double Dw =
		static_cast<double>(m_vertices[0][0]) * static_cast<double>(m_vertices[1][1]) * intersectPosition[2] +
		static_cast<double>(m_vertices[1][0]) * intersectPosition[1] * static_cast<double>(m_vertices[0][2]) +
		intersectPosition[0] * static_cast<double>(m_vertices[0][1]) * static_cast<double>(m_vertices[1][2]) -
		intersectPosition[0] * static_cast<double>(m_vertices[1][1]) * static_cast<double>(m_vertices[0][2]) -
		static_cast<double>(m_vertices[1][0]) * static_cast<double>(m_vertices[0][1]) * intersectPosition[2] -
		static_cast<double>(m_vertices[0][0]) * intersectPosition[1] * static_cast<double>(m_vertices[1][2]);

	double u = Du * rhD;
	double v = Dv * rhD;
	double w = Dw * rhD;

	// Outside
	if (u < 0.0 || v < 0.0 || w < 0.0)
	{
		outU = -1.0;
		outV = -1.0;
		outW = -1.0;

		return -1.0f;
	}

	outU = u;
	outV = v;
	outW = w;

	Logger::Log(LogType::INFO, Format("Intersection UVW = (%f, %f, %f)",
		outU, outV, outW));

	assert(fDistance >= 0.0f);
	SetSelected(true);
	
	Logger::Log(LogType::INFO, Format("Intersection position = (%f, %f, %f)", 
		intersectPosition.fX, intersectPosition.fY, intersectPosition.fZ));
	return fDistance;
}