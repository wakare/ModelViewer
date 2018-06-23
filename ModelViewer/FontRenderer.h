#pragma once
#include "FontImport.h"
#include "FontMesh.h"
#include "FontSubProcess.h"

class FontRenderer
{
private:
	FontImport m_importer;
	std::vector<FontMesh> m_fontMeshVec;
	std::shared_ptr<FontSubProcess> m_pFontSubProcess;

	int m_nWindowWidth;
	int m_nWindowHeight;
	int m_nTextFieldWidth;
	int m_nTextFieldHeight;
	int m_nLineHeight;
	int m_nFontWidth;
	int m_nFontHeight;
	int m_nCurrentLinesCount;
	bool m_bIsOpenLogRender;

public:
	FontRenderer();
	bool Init();
	void AfterInit();
	void AddString(std::string sText);
	void Update();
	void Clear();
	void OpenLog();
	void CloseLog();
	void OpenOrCloseLog();
};