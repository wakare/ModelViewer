#pragma once
#include "FontImport.h"
#include "FontMesh.h"
class FontRenderer
{
private:
	FontImport m_importer;
	std::vector<FontMesh> m_fontMeshVec;

	int m_nWindowWidth;
	int m_nWindowHeight;
	int m_nTextFieldWidth;
	int m_nTextFieldHeight;
	int m_nLineHeight;
	int m_nFontWidth;
	int m_nFontHeight;
	int m_nCurrentLinesCount;

public:
	FontRenderer();
	void AddString(std::string sText);
	void Update();
	void Render();
	void Clear();
};