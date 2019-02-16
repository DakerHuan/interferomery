#pragma once
#include "afxwin.h"
#include "interferomeryDlg.h"
#include "PrintView.h"
class CPrintFrame :
	public CFrameWnd
{
public:
	CPrintFrame(CinterferomeryDlg* pOld);
	~CPrintFrame();
public:
	CinterferomeryDlg*    m_pOldWnd;        // 用于保存主对话框对象；
	CPrintView*              m_pView;              // 用于保存视图类对象；
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};

