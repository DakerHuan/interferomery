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
	CinterferomeryDlg*    m_pOldWnd;        // ���ڱ������Ի������
	CPrintView*              m_pView;              // ���ڱ�����ͼ�����
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};

