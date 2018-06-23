#include "OcTree.h"
#include "assert.h"
#include "Logger.h"

int OcTree::g_nMaxDepth = 10;
int OcTree::g_nOcTreeSubTreeSize = 8;

inline float _distanceSquare(const Vector3& v1, const Vector3& v2)
{
	return (v1.fX - v2.fX) * (v1.fX - v2.fX) + (v1.fY - v2.fY) * (v1.fY - v2.fY) + (v1.fZ - v2.fZ) * (v1.fZ - v2.fZ);
}

inline int _getIndexByXYZOffset(bool bIsPositiveX, bool bIsPositiveY, bool bIsPositiveZ)
{
	return (static_cast<int>(bIsPositiveX) | static_cast<int>(bIsPositiveY) << 1 | static_cast<int>(bIsPositiveZ) << 2);
}

inline void _getXYZOffsetFlagByIndex(int nIndex, bool& bOutIsPositiveX, bool& bOutIsPositiveY, bool& bOutIsPositiveZ)
{
	assert(0 <= nIndex && nIndex < OcTree::g_nOcTreeSubTreeSize);
	bOutIsPositiveX = ((nIndex & 0x1) == 0x1);
	bOutIsPositiveY = ((nIndex & 0x2) == 0x2);
	bOutIsPositiveZ = ((nIndex & 0x4) == 0x4);
}

OcTree::OcTree(int nCurrentDepth, std::shared_ptr<Vector3> centerPosition, float fSpaceHalfLength) :
	m_subOcTreeVec({}), m_pNodeDataVec(nullptr), m_bHasSubTree(false), m_nCurrentDepth(nCurrentDepth), m_centerPosition(centerPosition), m_fSpaceHalfLength(fSpaceHalfLength)
{

}

bool OcTree::_addNodeToVector(std::shared_ptr<I3DObject> pNode)
{
	if (!m_pNodeDataVec)
	{
		m_pNodeDataVec = std::make_shared < std::vector<std::shared_ptr<I3DObject>>>();
		if (!m_pNodeDataVec)
		{
			Logger::Log(LogType::ERR, "Create vector failed.");
			return false;
		}
	}

	m_pNodeDataVec->push_back(pNode);

	return true;
}

bool OcTree::_addNodeToSubTree(std::shared_ptr<I3DObject> pNode)
{
	int nIndex = _getSubIndexOfNode(pNode);

	assert(m_subOcTreeVec.size() > nIndex);
	if (m_subOcTreeVec.size() <= nIndex)
		return false;

	m_subOcTreeVec[nIndex]->AddNode(pNode);

	return true;
}

bool OcTree::_createSubOcTree()
{
	assert(!m_bHasSubTree && m_subOcTreeVec.size() == 0);
	if (m_bHasSubTree || m_subOcTreeVec.size() > 0)
		return false;

	for (int nIndex = 0; nIndex < 8; nIndex++)
	{
		bool bIsPositiveX = false;
		bool bIsPositiveY = false;
		bool bIsPositiveZ = false;

		_getXYZOffsetFlagByIndex(nIndex, bIsPositiveX, bIsPositiveY, bIsPositiveZ);

		float nSpaceLength = m_fSpaceHalfLength * 0.5f;

		auto pVector3 = std::make_shared<Vector3>(
			m_centerPosition->fX + (bIsPositiveX ? 1 : -1) * nSpaceLength,
			m_centerPosition->fY + (bIsPositiveY ? 1 : -1) * nSpaceLength,
			m_centerPosition->fZ + (bIsPositiveZ ? 1 : -1) * nSpaceLength
		);

		m_subOcTreeVec.push_back(std::make_shared<OcTree>(m_nCurrentDepth + 1, pVector3, nSpaceLength));
	}

	assert(m_subOcTreeVec.size() > 0);
	if (m_subOcTreeVec.size() <= 0)
		return false;

	m_bHasSubTree = true;

	return true;
}

int OcTree::_getSubIndexOfNode(std::shared_ptr<I3DObject> pNode)
{
	assert(pNode);
	int nResultIndex = -1;

	assert(m_subOcTreeVec.size() > 0);
	if (m_subOcTreeVec.size() <= 0)
		return nResultIndex;

	Vector3 avgCoord = { 0.0f, 0.0f, 0.0f };

	int nVertexCount = pNode->GetVertexCount();
	for (int nIndex = 0; nIndex < nVertexCount; nIndex++)
	{
		Vector3 vertexCoord = pNode->GetVertex3DCoordByIndex(nIndex);
		avgCoord.fX += vertexCoord.fX;
		avgCoord.fY += vertexCoord.fY;
		avgCoord.fZ += vertexCoord.fZ;
	}

	avgCoord.fX = avgCoord.fX / static_cast<float>(nVertexCount);
	avgCoord.fY = avgCoord.fY / static_cast<float>(nVertexCount);
	avgCoord.fZ = avgCoord.fZ / static_cast<float>(nVertexCount);

	// Calculate the nearest subTree index
	float fDistance = -1.0f;
	for (int nIndex = 0; nIndex < OcTree::g_nOcTreeSubTreeSize; nIndex++)
	{
		float fDistanceSquare = _distanceSquare(*m_subOcTreeVec[nIndex]->m_centerPosition, avgCoord);
		if (fDistance < 0.0f)
		{
			fDistance = fDistanceSquare;
			nResultIndex = nIndex;
			continue;
		}

		if (fDistance > fDistanceSquare)
		{
			fDistance = fDistanceSquare;
			nResultIndex = nIndex;
			continue;
		}
	}

	assert(0 <= nResultIndex && nResultIndex < OcTree::g_nOcTreeSubTreeSize);
	return nResultIndex;
}

bool OcTree::AddNode(std::shared_ptr<I3DObject> pNode)
{
	assert(pNode);
	assert(!(m_bHasSubTree && m_pNodeData));

	// Update OcTree range first
	for (int nIndex = 0; nIndex < pNode->GetVertexCount(); nIndex++)
	{
		auto vertex = pNode->GetVertex3DCoordByIndex(nIndex);
		if ((m_centerPosition->fX - m_fSpaceHalfLength) <= vertex.fX && vertex.fX <= (m_centerPosition->fX + m_fSpaceHalfLength) &&
			(m_centerPosition->fY - m_fSpaceHalfLength) <= vertex.fY && vertex.fY <= (m_centerPosition->fY + m_fSpaceHalfLength) &&
			(m_centerPosition->fZ - m_fSpaceHalfLength) <= vertex.fZ && vertex.fZ <= (m_centerPosition->fZ + m_fSpaceHalfLength))
			continue;

		Vector3 minCoord;
		Vector3 maxCoord;

		minCoord.fX = (m_centerPosition->fX - m_fSpaceHalfLength > vertex.fX) ? vertex.fX : m_centerPosition->fX - m_fSpaceHalfLength;
		minCoord.fY = (m_centerPosition->fY - m_fSpaceHalfLength > vertex.fY) ? vertex.fY : m_centerPosition->fY - m_fSpaceHalfLength;
		minCoord.fZ = (m_centerPosition->fZ - m_fSpaceHalfLength > vertex.fZ) ? vertex.fZ : m_centerPosition->fZ - m_fSpaceHalfLength;

		maxCoord.fX = (m_centerPosition->fX + m_fSpaceHalfLength < vertex.fX) ? vertex.fX : m_centerPosition->fX + m_fSpaceHalfLength;
		maxCoord.fY = (m_centerPosition->fY + m_fSpaceHalfLength < vertex.fY) ? vertex.fY : m_centerPosition->fY + m_fSpaceHalfLength;
		maxCoord.fZ = (m_centerPosition->fZ + m_fSpaceHalfLength < vertex.fZ) ? vertex.fZ : m_centerPosition->fZ + m_fSpaceHalfLength;

		m_centerPosition->fX = (maxCoord.fX + minCoord.fX) * 0.5f;
		m_centerPosition->fY = (maxCoord.fY + minCoord.fY) * 0.5f;
		m_centerPosition->fZ = (maxCoord.fZ + minCoord.fZ) * 0.5f;

		float fMaxLength = fmax(fmax(maxCoord.fX - minCoord.fX, maxCoord.fY - minCoord.fY), maxCoord.fZ - minCoord.fZ);
		m_fSpaceHalfLength = fMaxLength * 0.5f;
	}

	if (g_nMaxDepth < m_nCurrentDepth)
	{
		if(!_addNodeToVector(pNode))
			return false;

		return true;
	}

	if (!m_bHasSubTree && m_pNodeData != nullptr)
	{
		if (!_createSubOcTree())
			return false;

		auto _nIndex = _getSubIndexOfNode(m_pNodeData);
		assert(0 <= _nIndex && _nIndex < OcTree::g_nOcTreeSubTreeSize);
		if (_nIndex < 0 || _nIndex >= OcTree::g_nOcTreeSubTreeSize)
			return false;

		m_subOcTreeVec[_nIndex]->AddNode(m_pNodeData);

		_nIndex = _getSubIndexOfNode(pNode);
		assert(0 <= _nIndex && _nIndex < OcTree::g_nOcTreeSubTreeSize);
		if (_nIndex < 0 || _nIndex >= OcTree::g_nOcTreeSubTreeSize)
			return false;

		m_subOcTreeVec[_nIndex]->AddNode(pNode);
		m_pNodeData = nullptr;

		return true;
	}

	if (!m_bHasSubTree && m_pNodeData == nullptr)
	{
		m_pNodeData = pNode;
		return true;
	}

	assert(m_pNodeData == nullptr);
	if (m_bHasSubTree && m_pNodeData == nullptr)
	{
		if(!_addNodeToSubTree(pNode))
			return false;

		return true;
	}

	assert(false);
	return true;
}

bool OcTree::IsObjInsideAABB(Vector3 position, const Vector3& minCoord, const Vector3& maxCoord)
{
	bool bInsideX = minCoord.fX <= position.fX && position.fX <= maxCoord.fX;
	bool bInsideY = minCoord.fY <= position.fY && position.fY <= maxCoord.fY;
	bool bInsideZ = minCoord.fZ <= position.fZ && position.fZ <= maxCoord.fZ;

	return bInsideX && bInsideY && bInsideZ;
}

float OcTree::AABBIntersectWithRay(const Ray & ray)
{
	float fDistance = -1.0f;

	assert(m_centerPosition != nullptr && m_fSpaceHalfLength >= 0.0f);
	if (m_centerPosition == nullptr || m_fSpaceHalfLength < 0.0f)
		return fDistance;

	Vector3 minCoord = { m_centerPosition->fX - m_fSpaceHalfLength, m_centerPosition->fY - m_fSpaceHalfLength , m_centerPosition->fZ - m_fSpaceHalfLength };
	Vector3 maxCoord = { m_centerPosition->fX + m_fSpaceHalfLength, m_centerPosition->fY + m_fSpaceHalfLength , m_centerPosition->fZ + m_fSpaceHalfLength };

	fDistance = OcTree::CubeIntersectWithRay(ray, minCoord, maxCoord);
	
	return fDistance;
}

bool OcTree::GetOcTreeRangeCoord(Vector3& minCoord, Vector3& maxCoord)
{
	if (m_centerPosition == nullptr || m_fSpaceHalfLength < 0.0f)
		return false;

	minCoord = { m_centerPosition->fX - m_fSpaceHalfLength, m_centerPosition->fY - m_fSpaceHalfLength , m_centerPosition->fZ - m_fSpaceHalfLength };
	maxCoord = { m_centerPosition->fX + m_fSpaceHalfLength, m_centerPosition->fY + m_fSpaceHalfLength , m_centerPosition->fZ + m_fSpaceHalfLength };

	return true;
}

const std::shared_ptr<Vector3> OcTree::GetCenterPosition()
{
	return m_centerPosition;
}

float OcTree::CubeIntersectWithRay(const Ray & ray, const Vector3& minCoord, const Vector3& maxCoord)
{
	// Ray original position is inside
	if (IsObjInsideAABB(ray.m_startPosition, minCoord, maxCoord))
		return 0.0f;

	float fResult = -1.0f;

	bool bIsRayDirXZero = fabs(ray.m_direction.fX) < FLOAT_DEVIATION;
	bool bIsRayDirYZero = fabs(ray.m_direction.fY) < FLOAT_DEVIATION;
	bool bIsRayDirZZero = fabs(ray.m_direction.fZ) < FLOAT_DEVIATION;

	if (bIsRayDirXZero)
	{
		if (ray.m_startPosition.fX < minCoord.fX || ray.m_startPosition.fX > maxCoord.fX)
			return fResult;
	}

	if (bIsRayDirYZero)
	{
		if (ray.m_startPosition.fY < minCoord.fY || ray.m_startPosition.fY > maxCoord.fY)
			return fResult;
	}

	if (bIsRayDirZZero)
	{
		if (ray.m_startPosition.fZ < minCoord.fZ || ray.m_startPosition.fZ > maxCoord.fZ)
			return fResult;
	}

	float fRhDirectionX = (bIsRayDirXZero) ? FLOAT_INFINITE : 1.0f / ray.m_direction.fX;
	float fRhDirectionY = (bIsRayDirYZero) ? FLOAT_INFINITE : 1.0f / ray.m_direction.fY;
	float fRhDirectionZ = (bIsRayDirZZero) ? FLOAT_INFINITE : 1.0f / ray.m_direction.fZ;

	float fMinX = (bIsRayDirXZero) ? -FLOAT_INFINITE : (minCoord.fX - ray.m_startPosition.fX) * fRhDirectionX;
	float fMaxX = (bIsRayDirXZero) ?  FLOAT_INFINITE : (maxCoord.fX - ray.m_startPosition.fX) * fRhDirectionX;
	float fMinY = (bIsRayDirYZero) ? -FLOAT_INFINITE : (minCoord.fY - ray.m_startPosition.fY) * fRhDirectionY;
	float fMaxY = (bIsRayDirYZero) ?  FLOAT_INFINITE : (maxCoord.fY - ray.m_startPosition.fY) * fRhDirectionY;
	float fMinZ = (bIsRayDirZZero) ? -FLOAT_INFINITE : (minCoord.fZ - ray.m_startPosition.fZ) * fRhDirectionZ;
	float fMaxZ = (bIsRayDirZZero) ?  FLOAT_INFINITE : (maxCoord.fZ - ray.m_startPosition.fZ) * fRhDirectionZ;

	float fMinStep = fmax(fmax(fmin(fMinX, fMaxX), fmin(fMinY, fMaxY)), fmin(fMinZ, fMaxZ));
	float fMaxStep = fmin(fmin(fmax(fMinX, fMaxX), fmax(fMinY, fMaxY)), fmax(fMinZ, fMaxZ));

	// NoIntersect situation
	if (fMaxStep < 0.0f)
	{
		return fResult;
	}

	if (fMinStep > fMaxStep)
	{
		return fResult;
	}

	// Get nearest intersection point
	fResult = fMinStep;

	// Segment line length check
	if (fResult >= 0.0f && ray.m_bIsSegmentLine)
	{
		float fSegmentLineLength = -1.0f;
		if (!bIsRayDirXZero)
			fSegmentLineLength = (ray.m_endPosition.fX - ray.m_startPosition.fX) / ray.m_direction.fX;
		else if (!bIsRayDirYZero)
			fSegmentLineLength = (ray.m_endPosition.fY - ray.m_startPosition.fY) / ray.m_direction.fY;
		else if (!bIsRayDirZZero)
			fSegmentLineLength = (ray.m_endPosition.fZ - ray.m_startPosition.fZ) / ray.m_direction.fZ;
		
		assert(fSegmentLineLength > 0.0f);
		if (fSegmentLineLength <= 0.0f)
		{
			Logger::Log(LogType::FATAL, "SegmentLine length is zero.");
			return -1.0f;
		}

		// Over segment line length, intersection do not exist
		if (fSegmentLineLength < fResult)
			return -1.0f;
	}

	return fResult;
}
