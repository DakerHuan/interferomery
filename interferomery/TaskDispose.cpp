#include "stdafx.h"
#include "TaskDispose.h"

int TaskDispose::radius = 90;
int TaskDispose::x = 252;
int TaskDispose::y = 245;
int TaskDispose::height = 100;
int TaskDispose::width = 100;
int TaskDispose::rin = 2;
int TaskDispose::rout = 150;
int TaskDispose::aperType = 0;//0Բ�Σ�1���Σ�2��Բ��3����Σ�4����
int TaskDispose::length = 128;
bool TaskDispose::choosedone = false;
bool TaskDispose::LButDownAndMove = false;
bool TaskDispose::mouseacitive = false;
//bool TaskDispose::stopAperWnd = false;
int  TaskDispose::aperModel = 0;//�Զ�����ģʽ
int TaskDispose::backgroundCircle = 0;
bool TaskDispose::statechange = 0;
bool TaskDispose::staticanalysis = false;
Mat  TaskDispose::PtMap=Mat(512, 512, CV_8UC1, cv::Scalar::all(0));
Mat  TaskDispose::TaskSrcFrame = Mat(540, 720, CV_8UC1, cv::Scalar::all(0));
Point TaskDispose::origin = Point(104, 14);
Point TaskDispose::MultiCtrCen = Point(360, 256);
int TaskDispose::MultiCtrRadius = 208;
int TaskDispose::RButtonPos = 0;//0 Single left; 1 Single right;2 Multi left;3 Multi Right
int TaskDispose::SLData = 0;//0 PV ;1 PVrms; 2 POWER; 3 N;4 DeltaN ;5 ��Ȧ;6 ��˹/��
int TaskDispose::SRData = 2;//0 PV ;1 PVrms; 2 POWER; 3 N;4 DeltaN ;5 ��Ȧ;6 ��˹/��
int TaskDispose::SLUnit = 1;//0 fr;1 wave;2 um; 3 nm;
int TaskDispose::SRUnit = 1;//0 fr;1 wave;2 um; 3 nm;
int TaskDispose::MLData = 0;//0 PV ;1 PVrms; 2 POWER; 3 N;4 DeltaN ;5 ��Ȧ;6 ��˹/��
int TaskDispose::MRData = 2;//0 PV ;1 PVrms; 2 POWER; 3 N;4 DeltaN ;5 ��Ȧ;6 ��˹/��
int TaskDispose::MLUnit = 1;//0 fr;1 wave;2 um; 3 nm;
int TaskDispose::MRUnit = 1;//0 fr;1 wave;2 um; 3 nm;
//VideoCapture TaskDispose::vcap(0);
CString TaskDispose::tester = _T("����");
CString TaskDispose::productID = _T("111111");
//void vctPoint.


TaskDispose::TaskDispose()
{
}


TaskDispose::~TaskDispose()
{
}
