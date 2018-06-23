#pragma once
#include <memory>
#include <vector>
#include "WJMath.h"
#include "I3DObject.h"
#include "Ray.h"

class OcTree 
{
private:
	const int m_nCurrentDepth;
	float m_fSpaceHalfLength;
	const std::shared_ptr<Vector3> m_centerPosition;

	bool _addNodeToVector(std::shared_ptr<I3DObject>);
	bool _addNodeToSubTree(std::shared_ptr<I3DObject>);
	bool _createSubOcTree();
	int _getSubIndexOfNode(std::shared_ptr<I3DObject>);
public:
	static int g_nMaxDepth;
	static int g_nOcTreeSubTreeSize;

	bool m_bHasSubTree;

	std::shared_ptr<I3DObject>	m_pNodeData;
	std::vector<std::shared_ptr<OcTree>> m_subOcTreeVec;
	std::shared_ptr<std::vector<std::shared_ptr<I3DObject>>> m_pNodeDataVec;

	OcTree(int nCurrentDepth, std::shared_ptr<Vector3> centerPosition, float fSpaceHalfLength);
	
	bool AddNode(std::shared_ptr<I3DObject> p3DObject);
	bool GetOcTreeRangeCoord(Vector3& minCoord, Vector3& maxCoord);
	const std::shared_ptr<Vector3> GetCenterPosition();
	float AABBIntersectWithRay(const Ray& ray);
	
	static bool  IsObjInsideAABB(const Vector3& position, const Vector3& minCoord, const Vector3& maxCoord);
	static float CubeIntersectWithRay(const Ray& ray, const Vector3& minCoord, const Vector3& maxCoord);
	static float SphereIntersectWithRay(const Ray& ray, const Vector3& centerCoord, float fRadius);
};