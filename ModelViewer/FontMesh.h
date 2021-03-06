#pragma once
#include "ISceneObject.h"
class FontMesh : public ISceneObject
{
private:
	GLint m_nTexWidth;
	GLint m_nTexHeight;
	unsigned char* m_pTextureData;

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_textureObject;

public:
	FontMesh(int TexTriangleCount, int TexWidth, int TexHeight, unsigned char* pTextureData):
		m_nTriangleNumber(TexTriangleCount), m_nTexWidth(TexWidth), m_nTexHeight(TexHeight), m_pTextureData(pTextureData) {}

	int m_nTriangleNumber;

	PrimitiveData GetPrimitiveData();
	bool InitSpaceAccelerateStruct();
	void Update();
};