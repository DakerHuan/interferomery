
// interferomery.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "interferomery.h"
#include "interferomeryDlg.h"
#include "Registry.h"
#include "atlbase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CinterferomeryApp

BEGIN_MESSAGE_MAP(CinterferomeryApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CinterferomeryApp ����

CinterferomeryApp::CinterferomeryApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CinterferomeryApp ����

CinterferomeryApp theApp;


// CinterferomeryApp ��ʼ��

BOOL CinterferomeryApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	//int tLong;
	//try
	//{
	//	//��ȡ������Ѿ�ʹ�ô���HKEY_CURRENT_USER  
	//	//tLong = (Int32)Registry.GetValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\tryTimes", "UseTimes", 0);  
	//	tLong = (int)Registry.GetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", 0);
	//	MessageBox.Show("��л����ʹ����" + tLong + "��", "��ʾ", MessageBoxButtons.OK, MessageBoxIcon.Information);
	//}
	//catch
	//{
	//	//�״�ʹ�����  
	//	//Registry.SetValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\tryTimes", "UseTimes", 0, RegistryValueKind.DWord);  
	//	Registry.SetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", 0, RegistryValueKind.DWord);
	//	MessageBox.Show("��ӭ���û�ʹ�ñ����", "��ʾ", MessageBoxButtons.OK, MessageBoxIcon.Information);

	//}
	//tLong = (int)Registry.GetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", 0);
	//if (tLong < 30)
	//{
	//	int Times = tLong + 1;//��������ǵڼ���ʹ��  
	//						  //�����ʹ�ô���д��ע���  
	//						  //Registry.SetValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\tryTimes", "UseTimes", Times);  
	//	Registry.SetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", Times);
	//	//this.Close();//�رձ��β�������  

	//}
	//else
	//{
	//	MessageBox.Show("�Բ�������������ô����Ѵ�����,��ͨ������;����ȡע���룬����ע��ʹ�ã�", "����", MessageBoxButtons.OK, MessageBoxIcon.Warning);
	//	Application.Exit();//�ر�����Ӧ�ó��� 
	//}

	CRegistry reg;
	CString KeyName, KeyValue;
	KeyName = _T("Use Times");
	KeyValue = _T("0");
	if (!reg.OpenKey(CRegistry::currentUser, _T("Software\\my Application\\UseTimes")))//DEFAULT_REG_PATH:The path you want to open,like _T("Software\\Adobe\\Photoshop\\60.0")
	{
		reg.CreateKey(CRegistry::currentUser, _T("Software\\my Application\\UseTimes"));
		reg.SetValue(KeyName, KeyValue);
	}
	reg.GetValue(KeyName, KeyValue);
	int index = _ttoi(KeyValue);
	if (index>500)
	{
		AfxMessageBox(_T("���ý�����"));
		return FALSE;
	}
	index++;
	KeyValue.Format(_T("%d"), index);
	reg.SetValue(KeyName, KeyValue);
	reg.CloseKey();
	//HKEY hkey;
	//CString strCompany = _T("world");
	//CString strSeries = _T("1111-1111");
	//bool G_bIsLicesed = true;
	/*if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\my Application\\Settings\\Use Times"), 0, KEY_ALL_ACCESS, &hkey) != ERROR_SUCCESS)
	{
		CString initvalue = _T("0");
		VERIFY(!RegCreateKey(HKEY_CURRENT_USER, _T("Software\\my Application\\Settings"), &hkey));
		VERIFY(!RegSetValueEx(hkey, _T("Use Times"), 0, REG_SZ, (BYTE *)initvalue.GetBuffer(initvalue.GetLength()), 50));
	}*/
	//VERIFY(!RegSetValueEx(hkey, _T("LicesedBool"), 0, REG_DWORD, (BYTE *)&G_bIsLicesed, 4));
	
	//VERIFY(!RegSetValueEx(hkey, _T("Company Name"), 0, REG_SZ, (BYTE *)strCompany.GetBuffer(strCompany.GetLength()), 25));
	//VERIFY(!RegSetValueEx(hkey, _T("License Code"), 0, REG_SZ, (BYTE *)strSeries.GetBuffer(strSeries.GetLength()), 20));
	//RegCloseKey(hkey);

	CinterferomeryDlg dlg;
	m_pMainWnd = &dlg;
	//SetDialogBkColor(RGB(0, 0, 255), RGB(255, 0, 0));
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

