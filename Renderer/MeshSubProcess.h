#pragma once
#include "IRenderSubProcess.h"

class MeshSubProcess : public IRenderSubProcess
{
private:
	bool m_bIsInit;
	bool _initShaderMgr();
public:
	MeshSubProcess(): m_bIsInit(false) {}
	bool Init();
	void Render();
};