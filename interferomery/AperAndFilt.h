#pragma once
#include "afxwin.h"


// CAperAndFilt �Ի���

class CAperAndFilt : public CDialogEx
{
	DECLARE_DYNAMIC(CAperAndFilt)

public:
	CAperAndFilt(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAperAndFilt();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APERANDFILT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
