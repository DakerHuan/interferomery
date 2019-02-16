#include "stdafx.h"
#include "PrintView.h"
#include "PrintPreviewView.h"
BOOL CALLBACK _AfxPrintPreviewCloseProc(CFrameWnd* pFrameWnd)
{
	ASSERT_VALID(pFrameWnd);
	CPrintPreviewView* pView = (CPrintPreviewView*)pFrameWnd->GetDlgItem(AFX_IDW_PANE_FIRST);
	ASSERT_KINDOF(CPreviewView, pView);
	pView->OnPreviewClose();
	return FALSE;
}
CPrintView::CPrintView()
{
	m_nMapMode = MM_TEXT;
}

CPrintView::~CPrintView()
{
}
BEGIN_MESSAGE_MAP(CPrintView, CScrollView)
	ON_COMMAND(ID_FILE_PRINT, &CPrintView::OnFilePrint)
END_MESSAGE_MAP()


void CPrintView::OnFilePrint()
{
	// TODO: �ڴ���������������
}


BOOL CPrintView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  ���� DoPreparePrinting �Ե��á���ӡ���Ի���
	//pInfo->SetMaxPage(1);
	return DoPreparePrinting(pInfo);
	// return CLogScrollView::OnPreparePrinting(pInfo);
}
void CPrintView::OnFilePrintPreview()
{
	CPrintPreviewState* pState = new CPrintPreviewState;
	pState->lpfnCloseProc = _AfxPrintPreviewCloseProc; //���ô�ӡԤ�����ڹر�ʱ�ĵ��ú���
	if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this, RUNTIME_CLASS(CPrintPreviewView), pState))
	{
		TRACE0("Error, DoPrintPreview failed. \n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;
	}
}

void CPrintView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	// TODO: �������ͼ�ĺϼƴ�С
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


void CPrintView::OnDraw(CDC* /*pDC*/)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ����ר�ô����/����û���
}


void CPrintView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: �ڴ����ר�ô����/����û���
	//CLogScrollView::OnPrint(pDC, pInfo);

	CScrollView::OnPrint(pDC, pInfo);
}
