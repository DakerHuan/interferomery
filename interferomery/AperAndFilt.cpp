// AperAndFilt.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "interferomery.h"
#include "AperAndFilt.h"
#include "afxdialogex.h"


// CAperAndFilt �Ի���

IMPLEMENT_DYNAMIC(CAperAndFilt, CDialogEx)

CAperAndFilt::CAperAndFilt(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_APERANDFILT, pParent)
	, m_filtWnd(_T("3"))
	, m_enforcefilt(10)
	, m_outcutpercent(3)
	, m_incutpercent(5)
	, m_optimsize(3)
	, m_declearpar(0)
{
	m_declearpar = 0;
	
	//m_conbox.InsertString(0, _T("StringData"));
	
}

CAperAndFilt::~CAperAndFilt()
{
}

void CAperAndFilt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT5, m_filtWnd);
	DDX_Slider(pDX, IDC_SLIDER1, m_enforcefilt);
	DDX_Control(pDX, IDC_COMBO1, m_conbox);
	DDX_Text(pDX, IDC_EDIT1, m_outcutpercent);
	DDX_Text(pDX, IDC_EDIT2, m_incutpercent);
	DDX_Text(pDX, IDC_EDIT3, m_optimsize);
	DDX_Radio(pDX, IDC_RADIO1, m_declearpar);
}


BEGIN_MESSAGE_MAP(CAperAndFilt, CDialogEx)
END_MESSAGE_MAP()


// CAperAndFilt ��Ϣ�������


BOOL CAperAndFilt::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_conbox.AddString(_T("��"));
	m_conbox.AddString(_T("��"));
	m_conbox.AddString(_T("˫��"));
	m_conbox.SetCurSel(2);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
