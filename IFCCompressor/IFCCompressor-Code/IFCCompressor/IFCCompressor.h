
// IFCCompressor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CIFCCompressorApp:
// �йش����ʵ�֣������ IFCCompressor.cpp
//

class CIFCCompressorApp : public CWinApp
{
public:
	CIFCCompressorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CIFCCompressorApp theApp;