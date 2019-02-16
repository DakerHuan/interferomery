#include "stdafx.h"
#include "PrintFrame.h"

void SetLandscapeMode(int   PrintMode)
{
	PRINTDLG   pd;
	pd.lStructSize = (DWORD)sizeof(PRINTDLG);
	BOOL   bRet = AfxGetApp()->GetPrinterDeviceDefaults(&pd);
	if (bRet)
	{
		//   protect   memory   handle   with   ::GlobalLock   and   ::GlobalUnlock  
		DEVMODE   FAR   *pDevMode = (DEVMODE   FAR   *)::GlobalLock(pd.hDevMode);
		pDevMode->dmPaperSize = DMPAPER_A4;   //����ӡֽ����ΪA4   
											  //   set   orientation   to   landscape   
		if (PrintMode == 1)     //�����ӡ   
			pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		else   if (PrintMode == 2)    //�����ӡ   
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		::GlobalUnlock(pd.hDevMode);
	}
}
CPrintFrame::CPrintFrame(CinterferomeryDlg* pOld)
{
	m_pOldWnd = pOld;
	if (!Create(NULL, _T("��ӡԤ��"), WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CRect(200, 200, 500, 500)))
		TRACE0("Failed to create view window! \n");
}


CPrintFrame::~CPrintFrame()
{
	m_pOldWnd->ShowWindow(SW_SHOW);
}
BEGIN_MESSAGE_MAP(CPrintFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


int CPrintFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	CCreateContext context;
	context.m_pNewViewClass = RUNTIME_CLASS(CPrintView);
	context.m_pCurrentFrame = this;
	context.m_pCurrentDoc = NULL;
	context.m_pLastView = NULL;
	m_pView = STATIC_DOWNCAST(CPrintView, CreateView(&context));

	if (m_pView != NULL)
	{
		m_pView->ShowWindow(SW_SHOW);
		SetActiveView(m_pView);
		//   SetLandscapeMode(DMORIENT_LANDSCAPE);
	}

	SetIcon(m_pOldWnd->GetIcon(FALSE), FALSE);
	SetIcon(m_pOldWnd->GetIcon(TRUE), TRUE);
	ShowWindow(SW_MAXIMIZE);
	CWinApp *pApp = AfxGetApp();
	pApp->m_pMainWnd = this;
	//m_pView->OnFilePrintPreview();
	 m_pView->SendMessage(WM_COMMAND, ID_FILE_PRINT);  // ֱ�Ӵ�ӡ
	m_pOldWnd->ShowWindow(SW_HIDE);
	return 0;
}


void CPrintFrame::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CPrintFrame* pf = (CPrintFrame*)::AfxGetMainWnd();
	CWinApp *pApp = AfxGetApp();
	pApp->m_pMainWnd = pf->m_pOldWnd;
	pf->DestroyWindow();
	//CFrameWnd::OnClose();
}
