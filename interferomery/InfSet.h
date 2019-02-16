#pragma once


// InfSet 对话框

class InfSet : public CDialogEx
{
	DECLARE_DYNAMIC(InfSet)

public:
	InfSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~InfSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INFSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_List1Data;
	int m_List1Unit;
	int m_List2Data;
	int m_List2Unit;
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio7();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio12();
	afx_msg void OnBnClickedRadio13();
	afx_msg void OnBnClickedRadio14();
	afx_msg void OnBnClickedRadio15();
	afx_msg void OnBnClickedRadio17();
	afx_msg void OnBnClickedRadio16();
	afx_msg void OnBnClickedRadio18();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_strcompany;
	CString m_strtime;
	CString m_tester;
	CString m_prodectID;
};
