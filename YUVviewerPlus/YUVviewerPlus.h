
// YUVviewerPlus.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CYUVviewerPlusApp:
// �йش����ʵ�֣������ YUVviewerPlus.cpp
//

class CYUVviewerPlusApp : public CWinAppEx
{
public:
	CYUVviewerPlusApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CYUVviewerPlusApp theApp;