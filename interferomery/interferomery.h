
// interferomery.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CinterferomeryApp: 
// �йش����ʵ�֣������ interferomery.cpp
//

class CinterferomeryApp : public CWinApp
{
public:
	CinterferomeryApp();

// ��д
public:
	virtual BOOL InitInstance();
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CinterferomeryApp theApp;