#pragma once
#include <iostream>
#include <fstream>
#include <vector>

struct Font
{
	int index;
	int width;
	int height;

	float Umin;
	float Umax;
	float Vmin;
	float Vmax;
};

// Need FNT & PNG file
class FontImport
{
private:
	bool _loadBMPFile(const char* szBMPPath, int * nWidth, int * nHeight);
	void _loadTextureData(const char* szFilePath, int * nWidth, int * nHeight);

public:
	bool m_bBeginLoadInBottom;
	int m_nTextureWidth;
	int m_nTextureHeight;
	unsigned char* m_pTextureData;
	std::vector<Font> m_fontVec;

	FontImport() : m_fontVec(256) {  }
	~FontImport() { if (m_pTextureData) delete m_pTextureData; m_pTextureData = nullptr; }
	bool LoadFontFile(const char* pFNTFile, const char* pTextureFile);
	bool LoadFontFile(const char* pEqualWideTextureFile);
};