#include "stdafx.h"
#include "PrintPreviewView.h"

#include "stdafx.h"
//#include ".\mypreviewview.h"
//#include "StatisticsCoverDlg.h"
//#include ".\statisticscoverdlg.h"
#include  "PrintFrame.h"

#ifdef _DEBUG //���������_DEBUG
#define new DEBUG_NEW //����newΪDEBUG_NEW
#undef THIS_FILE //�����壬�����THIS_FILE�ĺ궨��
static char THIS_FILE[] = __FILE__;
#endif//�����궨��   
IMPLEMENT_DYNCREATE(CPrintPreviewView, CPreviewView)
CPrintPreviewView::CPrintPreviewView()
{

}
CPrintPreviewView::~CPrintPreviewView()
{

}
BEGIN_MESSAGE_MAP(CPrintPreviewView, CPreviewView)

	ON_COMMAND(AFX_ID_PREVIEW_CLOSE, OnPreviewClose)
	ON_COMMAND(AFX_ID_PREVIEW_PRINT, OnPreviewPrint)

END_MESSAGE_MAP()

void CPrintPreviewView::OnDraw(CDC *pDC)
{
	CPreviewView::OnDraw(pDC);

	m_pToolBar->PostMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);// ������������״̬����
}

void CPrintPreviewView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
	CPrintPreviewView::OnEndPrintPreview(pDC, pInfo, point, pView);
}

void CPrintPreviewView::OnPreviewClose()
{
	CPrintFrame* pf = (CPrintFrame*)::AfxGetMainWnd();
	CWinApp *pApp = AfxGetApp();
	pApp->m_pMainWnd = pf->m_pOldWnd;
	pf->DestroyWindow();
}

void CPrintPreviewView::OnPreviewPrint()
{
	m_pPrintView->SendMessage(WM_COMMAND, ID_FILE_PRINT);

}
