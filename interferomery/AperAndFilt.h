#pragma once
#include "afxwin.h"


// CAperAndFilt 对话框

class CAperAndFilt : public CDialogEx
{
	DECLARE_DYNAMIC(CAperAndFilt)

public:
	CAperAndFilt(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAperAndFilt();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APERANDFILT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_filtWnd;
	int m_enforcefilt;
	CComboBox m_conbox;
	int m_outcutpercent;
	int m_incutpercent;
	int m_optimsize;
	int m_declearpar;
	virtual BOOL OnInitDialog();
};
