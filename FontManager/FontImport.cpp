#include <string>
#include <assert.h>

#include "FontImport.h"
#include "SOIL.h"
#include "Logger.h"

bool FontImport::_loadBMPFile(const char * szBMPPath, int * nWidth, int * nHeight)
{
	// Only load 32-bit BMP file
	std::ifstream bmpStream(szBMPPath, std::ios::in | std::ios::binary);
	if (!bmpStream.is_open())
		return false;

	char pTemp[4];
	memset(pTemp, 0, 4);

	// Skip BITMAP Header info
	bmpStream.ignore(14);

	// Skip BITMAP Info size
	bmpStream.ignore(4);

	// Get width & height
	bmpStream.read(pTemp, 4);
	memcpy(nWidth, pTemp, 4);
	memset(pTemp, 0, 4);

	bmpStream.read(pTemp, 4);
	memcpy(nHeight, pTemp, 4);
	memset(pTemp, 0, 4);

	m_bBeginLoadInBottom = nHeight > 0;

	int nBitDepth = 0;
	bmpStream.ignore(2);
	bmpStream.read(pTemp, 2);
	memcpy(&nBitDepth, pTemp, 4);
	memset(pTemp, 0, 4);

	if (nBitDepth < 24)
	{
		assert(false);
		return false;
	}

	// Skip other info header
	bmpStream.ignore(24);

	int nDataSize = (nBitDepth / 8) * (*nWidth) * (*nHeight);
	char* pData = (char*)malloc(nDataSize);
	bmpStream.read(pData, nDataSize);

	// TODO: swap R & B channel
	
	m_pTextureData = reinterpret_cast<unsigned char*>(pData);

	bmpStream.close();
	return true;
}

void FontImport::_loadTextureData(const char * szFilePath, int * nWidth, int * nHeight)
{
	m_pTextureData = SOIL_load_image(szFilePath, nWidth, nHeight, 0, SOIL_LOAD_RGBA);
	
	if (!_loadBMPFile(szFilePath, nWidth, nHeight))
		Logger::Log(LogType::ERR, "Load BMP file failed.");

	assert(m_pTextureData);
}

bool FontImport::LoadFontFile(const char* pEqualWideTextureFile)
{
	// Load a 256 * 256 file while each font size is 16 * 16, total 256 char
	int nWidth, nHeight;
	_loadTextureData(pEqualWideTextureFile, &nWidth, &nHeight);

	assert(nWidth == 256 && nHeight == 256);
	const float fUVOffset = 16.0f / 256.0f;
	m_nTextureWidth = 256;
	m_nTextureHeight = 256;

	for (int nIndexY = 0; nIndexY < 16; nIndexY++)
	{
		for (int nIndexX = 0; nIndexX < 16; nIndexX++)
		{
			Font font;

			font.index = 16 * nIndexY + nIndexX;
			font.Umin = nIndexX * fUVOffset;
			font.Umax = font.Umin + fUVOffset;
			font.Vmin = nIndexY * fUVOffset;
			font.Vmax = font.Vmin + fUVOffset;
			if (m_bBeginLoadInBottom)
			{
				font.Vmin = 1 - font.Vmin;
				font.Vmax = 1 - font.Vmax;
			}

			font.width = 16;
			font.height = 16;

			m_fontVec[font.index] = font;
		}
	}

	return true;
}

bool FontImport::LoadFontFile(const char* pFNTFile, const char * pTextureFile)
{
	const int valueMaxLength = 3;

	std::ifstream fileStream(pFNTFile);
	if (!fileStream.is_open())
		return false;

	char* pBuffer = (char*)malloc(sizeof(char) * 50);
	std::string temp;
	
	std::getline(fileStream, temp);
	std::getline(fileStream, temp);
	
	// Get texture size
	auto nWidthOffset = temp.find("scaleW=");
	auto sWidth = temp.substr(nWidthOffset + 7, valueMaxLength);
	m_nTextureWidth = std::atoi(sWidth.c_str());

	auto nHeightOffset = temp.find("scaleH=");
	auto sHeight = temp.substr(nHeightOffset + 7, valueMaxLength);
	m_nTextureHeight = std::atoi(sHeight.c_str());

	int nWidth = 0;
	int nHeight = 0;
	_loadTextureData(pTextureFile, &nWidth, &nHeight);

	std::getline(fileStream, temp);
	std::getline(fileStream, temp);

	int nOffset = temp.find('=');
	auto sCount = temp.substr(nOffset + 1, temp.length() - nOffset - 1);
	int nCount = std::atoi(sCount.c_str());
	int nCurrentIndex = 0;
	assert(nCount > 0);

	while (std::getline(fileStream, temp) && ((nCurrentIndex++) < nCount))
	{
		Font font;

		auto idIndex = temp.find("id=");
		auto valueStr = temp.substr(idIndex + 3, valueMaxLength);
		font.index = std::atoi(valueStr.c_str());

		auto xIndex = temp.find("x=");
		valueStr = temp.substr(xIndex + 2, valueMaxLength);
		font.Umin = std::atoi(valueStr.c_str()) / static_cast<float>(m_nTextureWidth);

		auto yIndex = temp.find("y=");
		valueStr = temp.substr(yIndex + 2, valueMaxLength);
		font.Vmin = std::atoi(valueStr.c_str()) / static_cast<float>(m_nTextureHeight);

		auto widthIndex = temp.find("width=");
		valueStr = temp.substr(widthIndex + 6, valueMaxLength);
		font.width = std::atoi(valueStr.c_str());

		auto heightIndex = temp.find("height=");
		valueStr = temp.substr(heightIndex + 7, valueMaxLength);
		font.height = std::atoi(valueStr.c_str());

		float fUOffset = font.width / static_cast<float>(m_nTextureWidth);
		float fVOffset = font.height / static_cast<float>(m_nTextureHeight);

		font.Umax = font.Umin + fUOffset;
		font.Vmax = font.Vmin + fVOffset;

		m_fontVec[font.index] = font;
	}

	return true;
}
