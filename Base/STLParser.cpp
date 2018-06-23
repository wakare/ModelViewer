#include "STLParser.h"
#include <iostream>
#include <fstream>
#include <assert.h>

STLParser::STLParser()
{
	memset(m_triangleCountBuffer, 0, 4);
}

STLParser::~STLParser()
{
	if (m_pTriangleData)
	{
		delete m_pTriangleData;
		m_pTriangleData = nullptr;
	}
}

bool STLParser::LoadDateFromFile(const char* szFilePath)
{
	/*
	STL file:
	1. file Header		-- 80 bytes
	2. triangles count	-- 4 bytes
	3. triangles data	-- 50 bytes
	a. float32[3] normal vector
	b. float32[3] vertex1
	c. float32[3] vertex2
	d. float32[3] vertex3
	e. reserver data (2 bytes)
	*/

	assert(szFilePath);

	std::ifstream m_fileStream(szFilePath, std::ios::in | std::ios::binary);
	if (!m_fileStream.is_open())
		return false;

	// Step.1 Read file header
	m_fileStream.ignore(80);

	// Step.2 Load triangles count
	m_fileStream.read(static_cast<char*>(m_triangleCountBuffer), sizeof(m_triangleCountBuffer));
	auto triCountPtr = reinterpret_cast<int*>(m_triangleCountBuffer);
	m_nTriangleCount = *triCountPtr;
	assert(m_nTriangleCount >= 0);

	// Step.3 Read triangle data
	const int nTriangleDataSize = sizeof(float) * 4 * 3 + 2;
	m_pTriangleData = (char*)malloc(m_nTriangleCount * nTriangleDataSize);
	m_fileStream.read(m_pTriangleData, m_nTriangleCount * nTriangleDataSize);

	// Close file resource
	m_fileStream.close();
		
	return true;
}

void STLParser::GetSTLData(char *& pTriangleData, int & nTriangleCount)
{
	pTriangleData = m_pTriangleData;
	nTriangleCount = m_nTriangleCount;
}