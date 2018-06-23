#pragma once
#include <memory>
#include "mesh.h"
#include "WJMath.h"

class STLParser
{
public:
	char	m_triangleCountBuffer[4];
	char*	m_pTriangleData;
	int		m_nTriangleCount;

	STLParser();
	~STLParser();
	bool LoadDateFromFile(const char* szFilePath);
	void GetSTLData(char*& pTriangleData, int& nTriangleSize);
};