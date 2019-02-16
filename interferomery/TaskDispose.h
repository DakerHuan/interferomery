#pragma once
#include "stdafx.h"
#include "afxwin.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/utility.hpp>
using namespace std;
#include<iostream> 
using namespace cv;
//static std::vector<cv::Point> vctPointglobal;
class TaskDispose
{
public:
	TaskDispose();
	~TaskDispose();
public:
	static int radius;
	static int x;
	static int y;
	static int height;
	static int width;
	static int rin;
	static int rout;
	static int aperType;//0圆形；1矩形；2椭圆；3多边形;4环形
	static int length;
	static bool choosedone;//手动选择区域完成
	static bool LButDownAndMove;
	static bool mouseacitive;
	static int backgroundCircle;
	static bool statechange;
	//static bool stopAperWnd;
	static int aperModel;//孔径模式，0 自动单件、1 自动多件、2手动
	static bool staticanalysis;//手动模式下静态分析
	static Mat PtMap;
	static Mat TaskSrcFrame;
	static Point origin;
	static Point MultiCtrCen;
	static int MultiCtrRadius;
	static int RButtonPos;
	static int SLData;
	static int SRData;
	static int SLUnit;
	static int SRUnit;
	static int MLData;
	static int MRData;
	static int MLUnit;
	static int MRUnit;
	static CString productID;
	static CString tester;
	//static VideoCapture vcap;

	//static std::vector<cv::Point> vctPoint;
};

