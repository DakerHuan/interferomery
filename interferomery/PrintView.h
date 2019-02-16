#pragma once
#include "afxwin.h"
class CPrintView :
	public CScrollView
{
public:
	CPrintView();
	~CPrintView();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFilePrint();
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
public:
	void OnFilePrintPreview();
    virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* /*pDC*/);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
};

