
// interferomery.cpp : 定义应用程序的类行为。
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


// CinterferomeryApp 构造

CinterferomeryApp::CinterferomeryApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CinterferomeryApp 对象

CinterferomeryApp theApp;


// CinterferomeryApp 初始化

BOOL CinterferomeryApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//int tLong;
	//try
	//{
	//	//获取软件的已经使用次数HKEY_CURRENT_USER  
	//	//tLong = (Int32)Registry.GetValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\tryTimes", "UseTimes", 0);  
	//	tLong = (int)Registry.GetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", 0);
	//	MessageBox.Show("感谢您已使用了" + tLong + "次", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
	//}
	//catch
	//{
	//	//首次使用软件  
	//	//Registry.SetValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\tryTimes", "UseTimes", 0, RegistryValueKind.DWord);  
	//	Registry.SetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", 0, RegistryValueKind.DWord);
	//	MessageBox.Show("欢迎新用户使用本软件", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);

	//}
	//tLong = (int)Registry.GetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", 0);
	//if (tLong < 30)
	//{
	//	int Times = tLong + 1;//计算软件是第几次使用  
	//						  //将软件使用次数写入注册表  
	//						  //Registry.SetValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\tryTimes", "UseTimes", Times);  
	//	Registry.SetValue("HKEY_CURRENT_USER\\SOFTWARE\\tryTimes", "UseTimes", Times);
	//	//this.Close();//关闭本次操作窗口  

	//}
	//else
	//{
	//	MessageBox.Show("对不起，您的免费试用次数已达上限,请通过正当途径获取注册码，进行注册使用！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
	//	Application.Exit();//关闭整个应用程序 
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
		AfxMessageBox(_T("试用结束！"));
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
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

