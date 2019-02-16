// InfSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "interferomery.h"
#include "InfSet.h"
#include "afxdialogex.h"
#include "TaskDispose.h"

// InfSet �Ի���

IMPLEMENT_DYNAMIC(InfSet, CDialogEx)

InfSet::InfSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INFSET, pParent)
	, m_List1Data(0)
	, m_List1Unit(0)
	, m_List2Data(0)
	, m_List2Unit(0)
	, m_strcompany(_T(""))
	, m_strtime(_T(""))
	, m_tester(_T(""))
	, m_prodectID(_T(""))
{
	
}

InfSet::~InfSet()
{
}

void InfSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_List1Data);
	DDX_Radio(pDX, IDC_RADIO8, m_List1Unit);
	DDX_Radio(pDX, IDC_RADIO12, m_List2Data);
	DDX_Radio(pDX, IDC_RADIO19, m_List2Unit);
	DDX_Text(pDX, IDC_COMPANY, m_strcompany);
	DDX_Text(pDX, IDC_TIME, m_strtime);
	DDX_Text(pDX, IDC_TESTER, m_tester);
	DDX_Text(pDX, IDC_PRODNUM, m_prodectID);
}


BEGIN_MESSAGE_MAP(InfSet, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO4, &InfSet::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO6, &InfSet::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO5, &InfSet::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO7, &InfSet::OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_RADIO1, &InfSet::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &InfSet::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &InfSet::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO12, &InfSet::OnBnClickedRadio12)
	ON_BN_CLICKED(IDC_RADIO13, &InfSet::OnBnClickedRadio13)
	ON_BN_CLICKED(IDC_RADIO14, &InfSet::OnBnClickedRadio14)
	ON_BN_CLICKED(IDC_RADIO15, &InfSet::OnBnClickedRadio15)
	ON_BN_CLICKED(IDC_RADIO17, &InfSet::OnBnClickedRadio17)
	ON_BN_CLICKED(IDC_RADIO16, &InfSet::OnBnClickedRadio16)
	ON_BN_CLICKED(IDC_RADIO18, &InfSet::OnBnClickedRadio18)
	ON_BN_CLICKED(IDOK, &InfSet::OnBnClickedOk)
END_MESSAGE_MAP()


// InfSet ��Ϣ�������


void InfSet::OnBnClickedRadio4()//N
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List1Unit = 0;
	m_List1Data = 3;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio6()//delta N
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List1Unit = 0;
	m_List1Data = 4;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List1Unit = 0;
	m_List1Data = 5;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List1Unit = 0;
	m_List1Data = 6;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_List1Data = 0;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_List1Data = 1;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_List1Data = 2;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio12()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_List2Data = 0;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio13()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_List2Data = 1;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio14()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	m_List2Data = 2;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio15()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List2Unit = 0;
	m_List2Data = 3;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio17()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List2Unit = 0;
	m_List2Data = 4;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio16()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List2Unit = 0;
	m_List2Data = 5;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedRadio18()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_List2Unit = 0;
	m_List2Data = 6;
	UpdateData(FALSE);
}


void InfSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	TaskDispose::SLData = m_List1Data;
	TaskDispose::SLUnit = m_List1Unit;
	TaskDispose::SRData = m_List2Data;
	TaskDispose::SRUnit = m_List2Unit;
	TaskDispose::productID = m_prodectID;
	TaskDispose::tester = m_tester;
	CDialogEx::OnOK();
}


BOOL InfSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_List1Data = TaskDispose::SLData;
	m_List1Unit = TaskDispose::SLUnit;
	m_List2Data = TaskDispose::SRData;
	m_List2Unit = TaskDispose::SRUnit;
	m_strcompany = _T("�Ϻ�������");
	m_prodectID=TaskDispose::productID;
	m_tester=TaskDispose::tester;
	/*SYSTEMTIME st;
	CString strDate, strTime;
	GetLocalTime(&st);
	strDate.Format("%4d-%2d-%2d", st.wYear, st.wMonth, st.wDay);
	strTime.Format("%2d:%2d:%2d", st.wHour, st.wMinute, st.wSecond);
	m_strtime = strDate + strTime;*/
	CString str; //��ȡϵͳʱ�� ����
	CTime tm; tm = CTime::GetCurrentTime();
	str = tm.Format("%Y��%m��%d�� %X");
	m_strtime = str;
	UpdateData(false);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
