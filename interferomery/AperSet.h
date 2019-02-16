#pragma once


// AperSet 对话框
#include "TaskDispose.h"
#include "afxwin.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include<iostream> 
using namespace cv;
using namespace std;
//#include <cv.h>  
//#include <cxcore.h>  
//#include <highgui.h> 
#include "windows.h"
class AperSet : public CDialogEx
{
	DECLARE_DYNAMIC(AperSet)

public:
	AperSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AperSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APERSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	int m_x;
	int m_y;
	int m_radius;
	int m_height;
	int m_width;
	int m_rout;
	int m_rin;
	virtual BOOL OnInitDialog();
	int m_iRadio1;
	//void thread();
	static int thread(LPVOID lpParameter);
	CWinThread *m_pthread;
	LRESULT AperSet::OnUpdateData(WPARAM wParam, LPARAM lParam);
	LRESULT AperSet::OnUpdateView(WPARAM wParam, LPARAM lParam);
	void  CreateEffectPtMap();
	int m_heightellipse;
	int m_widthellipse;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedButtonup();
	afx_msg void OnBnClickedButtondown();
	afx_msg void OnBnClickedButtonleft();
	afx_msg void OnBnClickedButtonright();
	int m_step;
	
};
