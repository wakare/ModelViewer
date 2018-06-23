#include <assert.h>
#include <windows.h>
#include <iostream>
#include "Mesh.h"
#include "Logger.h"
#include "Timer.h"

inline float _nextFloatFromBuffer(char* pTriangleData, int& nCurrentPos, char* pBuffer)
{
	memcpy(pBuffer, pTriangleData + nCurrentPos, 4);
	nCurrentPos += 4;
	return *reinterpret_cast<float*>(pBuffer);
}

void Mesh::_moveToScreenCenter()
{
	auto centerPosition = m_pOcTree->GetCenterPosition();
	m_modelTranslateTransform.Translate(-centerPosition->fX, -centerPosition->fY, -centerPosition->fZ);
}

Mesh::Mesh():
	m_nTriangleNumber(0), m_defaultColor(1.0f, 1.0f, 1.0f)
{

}

Mesh::~Mesh()
{
	if (m_primitiveData.m_pfVertexData)
	{
		delete[] m_primitiveData.m_pfVertexData;
		m_primitiveData.m_pfVertexData = nullptr;
	}
}

bool Mesh::InitFromSTLData(char* pTriangleData, int nTriangleSize)
{
	assert(nTriangleSize > 0);

	bool bRetCode = false;
	char* pTempBuffer = (char*)malloc(sizeof(char) * 4);
	assert(pTempBuffer);
	int nCurrentPos = 0;

	m_nTriangleNumber = nTriangleSize;
	for (int index = 0; index < nTriangleSize; index++)
	{
		std::shared_ptr<TrianglePrimitive> pTempTriangle = std::make_shared<TrianglePrimitive>();
		pTempTriangle->m_normalVec[0] = _nextFloatFromBuffer(pTriangleData, nCurrentPos, pTempBuffer);
		pTempTriangle->m_normalVec[1] = _nextFloatFromBuffer(pTriangleData, nCurrentPos, pTempBuffer);
		pTempTriangle->m_normalVec[2] = _nextFloatFromBuffer(pTriangleData, nCurrentPos, pTempBuffer);

		for (int i = 0; i < 3; i++)
		{
			pTempTriangle->m_vertices[i][0] = _nextFloatFromBuffer(pTriangleData, nCurrentPos, pTempBuffer);
			pTempTriangle->m_vertices[i][1] = _nextFloatFromBuffer(pTriangleData, nCurrentPos, pTempBuffer);
			pTempTriangle->m_vertices[i][2] = _nextFloatFromBuffer(pTriangleData, nCurrentPos, pTempBuffer);
		}

		// Ignore reserver data (2 bytes)
		nCurrentPos += 2;

		m_p3DObjectVec.push_back(pTempTriangle);
	}

	delete pTempBuffer;
	bRetCode = true;

	return bRetCode;
}

PrimitiveData Mesh::GetPrimitiveData()
{
	if (m_primitiveData.m_pfVertexData == nullptr)
	{
		m_primitiveData.m_nVertexCount = static_cast<int>(m_nTriangleNumber * 3);
		m_primitiveData.m_nFloatCountPerVertex = 6;

		// Vertex = {X, Y, Z, nX, nY, nZ} -- total 6 floats
		m_primitiveData.m_pfVertexData = (float*)malloc(6 * m_primitiveData.m_nVertexCount * sizeof(float));
		for (unsigned index = 0; index < m_nTriangleNumber; index++)
		{
			auto pTempTriangle = m_p3DObjectVec[index];

			m_primitiveData.m_pfVertexData[18 * index]	  = pTempTriangle->GetVertex3DCoordByIndex(0).fX;
			m_primitiveData.m_pfVertexData[18 * index + 1] = pTempTriangle->GetVertex3DCoordByIndex(0).fY;
			m_primitiveData.m_pfVertexData[18 * index + 2] = pTempTriangle->GetVertex3DCoordByIndex(0).fZ;

			m_primitiveData.m_pfVertexData[18 * index + 3] = pTempTriangle->GetNormal().fX;
			m_primitiveData.m_pfVertexData[18 * index + 4] = pTempTriangle->GetNormal().fY;
			m_primitiveData.m_pfVertexData[18 * index + 5] = pTempTriangle->GetNormal().fZ;
			
			m_primitiveData.m_pfVertexData[18 * index + 6] = pTempTriangle->GetVertex3DCoordByIndex(1).fX;
			m_primitiveData.m_pfVertexData[18 * index + 7] = pTempTriangle->GetVertex3DCoordByIndex(1).fY;
			m_primitiveData.m_pfVertexData[18 * index + 8] = pTempTriangle->GetVertex3DCoordByIndex(1).fZ;

			m_primitiveData.m_pfVertexData[18 * index + 9] = pTempTriangle->GetNormal().fX;
			m_primitiveData.m_pfVertexData[18 * index + 10] = pTempTriangle->GetNormal().fY;
			m_primitiveData.m_pfVertexData[18 * index + 11] = pTempTriangle->GetNormal().fZ;

			m_primitiveData.m_pfVertexData[18 * index + 12] = pTempTriangle->GetVertex3DCoordByIndex(2).fX;
			m_primitiveData.m_pfVertexData[18 * index + 13] = pTempTriangle->GetVertex3DCoordByIndex(2).fY;
			m_primitiveData.m_pfVertexData[18 * index + 14] = pTempTriangle->GetVertex3DCoordByIndex(2).fZ;

			m_primitiveData.m_pfVertexData[18 * index + 15] = pTempTriangle->GetNormal().fX;
			m_primitiveData.m_pfVertexData[18 * index + 16] = pTempTriangle->GetNormal().fY;
			m_primitiveData.m_pfVertexData[18 * index + 17] = pTempTriangle->GetNormal().fZ;
		}
	}
	return m_primitiveData;
}

bool Mesh::InitSpaceAccelerateStruct()
{
	Timer::BeginTimer();
	Logger::Log(LogType::INFO, "Enter Mesh::InitSpaceAccelerateStruct");

	bool bResult = false;

	float fMinX = 0.0f;
	float fMinY = 0.0f;
	float fMinZ = 0.0f;
	float fMaxX = 0.0f;
	float fMaxY = 0.0f;
	float fMaxZ = 0.0f;
	bool bIsFirstVertex = true;

	for (auto it3DObject : m_p3DObjectVec)
	{
		for (int i = 0; i < it3DObject->GetVertexCount(); i++)
		{
			auto referenceVertexCoord = it3DObject->GetVertex3DCoordByIndex(i);

			if (bIsFirstVertex)
			{
				fMinX = referenceVertexCoord.fX;
				fMinY = referenceVertexCoord.fY;
				fMinZ = referenceVertexCoord.fZ;

				fMaxX = fMinX;
				fMaxY = fMinY;
				fMaxZ = fMinZ;

				bIsFirstVertex = false;
				continue;
			}

			fMinX = (referenceVertexCoord.fX < fMinX) ? referenceVertexCoord.fX : fMinX;
			fMinY = (referenceVertexCoord.fY < fMinY) ? referenceVertexCoord.fY : fMinY;
			fMinZ = (referenceVertexCoord.fZ < fMinZ) ? referenceVertexCoord.fZ : fMinZ;

			fMaxX = (referenceVertexCoord.fX > fMaxX) ? referenceVertexCoord.fX : fMaxX;
			fMaxY = (referenceVertexCoord.fY > fMaxY) ? referenceVertexCoord.fY : fMaxY;
			fMaxZ = (referenceVertexCoord.fZ > fMaxZ) ? referenceVertexCoord.fZ : fMaxZ;
		}
	}

	assert(m_pOcTree == nullptr);
	if (m_pOcTree)
		return false;

	std::shared_ptr<Vector3> centerPosition = std::make_shared<Vector3>((fMinX + fMaxX) * 0.5f, (fMinY + fMaxY) * 0.5f, (fMinZ + fMaxZ) * 0.5f);
	float fSpaceHalfLength = fmax(fmax(fMaxX - fMinX, fMaxY - fMinY), fMaxZ - fMinZ) * 0.5f;

	// ISceneObject default depth equals 1
	m_pOcTree = std::make_shared<OcTree>(1, centerPosition, fSpaceHalfLength);

	// Add Node To OcTree
	for (auto it3DObject : m_p3DObjectVec)
	{
		if (!m_pOcTree->AddNode(it3DObject))
			return false;
	}

	bResult = true;
	Logger::Log(LogType::DEBUG, Format("Mesh::InitSpaceAccelerateStruct cost time: %f ms", Timer::EndTimer() * 1000.0f));
	
	_moveToScreenCenter();

	return bResult;
}

bool Mesh::InitShaderMgr()
{
	m_pShaderMgr = std::make_shared<ShaderMgr>("Resources/VertexShader.glsl", "Resources/FragmentShader.glsl");
	assert(m_pShaderMgr!= nullptr);

	if (!m_pShaderMgr->Init("Resources/VertexShader.glsl", "Resources/FragmentShader.glsl"))
		return false;

	if (!m_pShaderMgr->CompileShader())
		return false;

	if (!m_pShaderMgr->LinkProgram())
		return false;

	return true;
}

void Mesh::Render(Transform viewTransform, Transform projTransform)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glUseProgram(m_pShaderMgr->GetShaderProgram());
	glBindVertexArray(m_VAO);

	auto modelTransform = m_modelRotateTransform * m_modelTranslateTransform;
	ShaderMgr::SetUniformVariableMatrix4(m_pShaderMgr->GetShaderProgram(), "modelMatrix", modelTransform.GetTransformMatrix());
	ShaderMgr::SetUniformVariableMatrix4(m_pShaderMgr->GetShaderProgram(), "viewMatrix", viewTransform.GetTransformMatrix());
	ShaderMgr::SetUniformVariableMatrix4(m_pShaderMgr->GetShaderProgram(), "projectionMatrix", projTransform.GetTransformMatrix());
	ShaderMgr::SetUniformVariableFloat3(m_pShaderMgr->GetShaderProgram(), "vertexColor", (m_bSelected) ? Vector3(1.0f, 0.0f, 0.0f) : m_defaultColor);

	glDrawArrays(GL_TRIANGLES, 0, m_primitiveData.m_nVertexCount * m_primitiveData.m_nFloatCountPerVertex);

	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
}

bool Mesh::InitGLData()
{
	if (!InitShaderMgr())
		return false;

	this->GetPrimitiveData();

	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);
	glUseProgram(m_pShaderMgr->GetShaderProgram());

	// Create a buffer to store vertex.
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Bind the triangle vertex data to buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_primitiveData.m_nFloatCountPerVertex * m_primitiveData.m_nVertexCount, m_primitiveData.m_pfVertexData, GL_STATIC_DRAW);

	// Set position vector format
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set normal vector format
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

	return true;
}

void Mesh::Update()
{
	// m_modelRotateTransform.Rotate(1.0f * PI / 180.0f, {1.0f, 1.0f, 0.0f});
}
