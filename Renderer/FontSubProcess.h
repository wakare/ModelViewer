#pragma once
#include "IRenderSubProcess.h"
#include "GLTextureUnit.h"

class FontSubProcess : public IRenderSubProcess
{
private:
	bool m_bIsInit;
	GLuint m_textureObject;
	GLint m_nTexWidth;
	GLint m_nTexHeight;
	GLvoid* m_pTextureData;
	std::shared_ptr<GLTextureUnit> m_pTextureUnit;

	bool _initTexture();
	bool _initShaderMgr();
public:
	FontSubProcess(GLint nWidth, GLint nHeight, GLvoid* pData): 
		m_bIsInit(false), m_nTexWidth(nWidth), m_nTexHeight(nHeight), m_pTextureData(pData), m_pTextureUnit(nullptr)
	{
	}

	bool Init();
	void Render();
};