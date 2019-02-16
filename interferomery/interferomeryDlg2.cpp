
// interferomeryDlg.cpp : 实现文件
//
#include <iostream>
#include "stdafx.h"
#include "interferomery.h"
#include "interferomeryDlg.h"
#include "afxdialogex.h"
#include "zernike.h" 
#include "putText.h"
#include "PrintFrame.h"
#include "AperAndFilt.h"
//#include "ExcelFormat.h"
//
//using namespace ExcelFormat;

//#pragma comment(lib，"ExcelDll.lib")  
//#define filePath "myTry.xls"
#include "CApplication.h"
#include "CFont0.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
using namespace std;
using namespace cv;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE HThreadGetCapture;//获取视频线程
RNG g_rng(12345);
#define WINDOW_NAME "【手动选择口径窗口】"
Mat srcframe, waitforprocframe,showsrcframe;
float PV = 0;
float POWER = 0;
float PVrms = 0;
bool stop = false;//程序停止运行
struct CenterPoint
{
	int x = 0;
	int y = 0;
	int radius = 0;
};
void find_beiji_Region0(Mat& Src, int AreaLimit, int &beiji_count, vector<vector<Point2i>> &beiji_blob)
{
	int RemoveCount = 0;       //记录除去的个数  
	//记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查  
	Mat Pointlabel = Mat::zeros(Src.size(), CV_8UC1);
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iData = Src.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iData[j] < 10)
			{
				iLabel[j] = 3;
			}
		}
	}

	vector<Point2i> NeihborPos;  //记录邻域点位置  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	NeihborPos.push_back(Point2i(-1, -1));
	NeihborPos.push_back(Point2i(-1, 1));
	NeihborPos.push_back(Point2i(1, -1));
	NeihborPos.push_back(Point2i(1, 1));

	int NeihborCount = 8;
	int CurrX = 0, CurrY = 0;
	//开始检测  
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 0)
			{
				//********开始该点处的检查**********  
				vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点  
				GrowBuffer.push_back(Point2i(j, i));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出  

				for (int z = 0; z < GrowBuffer.size(); z++)
				{

					for (int q = 0; q < NeihborCount; q++)                                      //检查四个邻域点  
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX < Src.cols&&CurrY >= 0 && CurrY < Src.rows)  //防止越界  
						{
							if (Pointlabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer  
								Pointlabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查  
							}
						}
					}

				}
				if (GrowBuffer.size() > AreaLimit )
				{	
						beiji_blob.push_back(GrowBuffer);
						beiji_count++;
					
				}
			}
		}
	}
}

void find_beiji_Region(Mat& Src, int AreaLimit, int &beiji_count, vector<vector<Point2i>> &beiji_blob)
{
	int RemoveCount = 0;       //记录除去的个数  
	//记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查  
	Mat Pointlabel = Mat::zeros(Src.size(), CV_8UC1);
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iData = Src.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iData[j] < 10)
			{
				iLabel[j] = 3;
			}
		}
	}
	vector<Point2i> NeihborPos;  //记录邻域点位置  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	NeihborPos.push_back(Point2i(-1, -1));
	NeihborPos.push_back(Point2i(-1, 1));
	NeihborPos.push_back(Point2i(1, -1));
	NeihborPos.push_back(Point2i(1, 1));

	int NeihborCount = 8;
	int CurrX = 0, CurrY = 0;
	//开始检测  
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 0)
			{
				//********开始该点处的检查**********  
				vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点  
				GrowBuffer.push_back(Point2i(j, i));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出  

				for (int z = 0; z < GrowBuffer.size(); z++)
				{

					for (int q = 0; q < NeihborCount; q++)                                      //检查四个邻域点  
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX < Src.cols&&CurrY >= 0 && CurrY < Src.rows)  //防止越界  
						{
							if (Pointlabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer  
								Pointlabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查  
							}
						}
					}

				}
				if (GrowBuffer.size() > AreaLimit /*&&GrowBuffer.size() < 70000*/)
				{
					vector<Point> points;
					for (int i = 0; i < GrowBuffer.size(); i++)
					{
						Point point;
						point.x = GrowBuffer[i].x;
						point.y = GrowBuffer[i].y;
						points.push_back(point);
					}
					RotatedRect bouding = minAreaRect(Mat(points));
					if (bouding.size.height > 50 || bouding.size.width > 50)         //高宽都低于50的特征blob都滤掉
					{
						beiji_blob.push_back(GrowBuffer);
						beiji_count++;
					}
				}
			}
		}
	}
}

bool CaculationUpdataCir(Mat &tmpMat, vector<vector<Point2i>> beiji_blob, Point &Currentcenter, int &Currentradius)
{
	if (beiji_blob.size() == 0) return false;

	for (int i = 0; i < beiji_blob.size(); i++)
	{
		vector<Point2i> tmpPoint;
		tmpPoint = beiji_blob[i];
		for (int j = 0; j < tmpPoint.size(); j++)
		{
			Point dots;
			dots.x = tmpPoint[j].x;
			dots.y = tmpPoint[j].y;
			circle(tmpMat, dots, 0, 255, 1, 4);
		}
	}
	//imwrite("F:/TmpMat1.bmp", srcImage);

	//利用凸包检测+外接最小矩形-》得到最大内接圆
	vector<vector<Point2i> > allContour;
	vector<Point2i> Contour;
	vector<Point> points;//点值
	findContours(tmpMat, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < allContour.size(); i++)
	{
		Contour = allContour[i];
		for (int j = 0; j < Contour.size(); j++)
		{
			Point point;
			point.x = Contour[j].x;
			point.y = Contour[j].y;
			points.push_back(point);
		}
	}

	vector<int> hull;
	convexHull(Mat(points), hull, false, true);
	tmpMat.setTo(0);
	int hullcount = hull.size();                         //凸包的边数
	if (hullcount == 0) return false;
	if (hullcount > 0)
	{
		Point point0;
		point0 = points[(hull[hullcount - 1])];          //连接凸包边的坐标点
		for (int i = 0; i < hullcount; i++)
		{
			Point point1;
			point1 = points[(hull[i])];
			line(tmpMat, point0, point1, 255, 1, 8);   //将凸包点连接起来形成内边缘的外圈轨迹
			point0 = point1;
		}
	}

	allContour.clear();
	Contour.clear();
	points.clear();
	findContours(tmpMat, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (allContour.size() != 1) return false;
	Contour = allContour[0];
	for (int j = 0; j < Contour.size(); j++)
	{
		Point point;
		point.x = Contour[j].x;
		point.y = Contour[j].y;
		points.push_back(point);
	}
	RotatedRect bouding = minAreaRect(Mat(points));
	Currentcenter = bouding.center;
	Currentradius = min(bouding.size.width, bouding.size.height) / 2;
	return true;
}

bool maxCircleUpdata(Mat &srcImage, Mat &workMask2, int &radius, Point &center)
{
	if (srcImage.empty() || workMask2.empty()) return false;
	Point CurrentCenter;
	int Currentradius;
	Mat dst, tmpImage0, tmpImage1;

	cv::pyrMeanShiftFiltering(srcImage, dst, 20, 40, 2);
	cv::cvtColor(dst, tmpImage1, COLOR_BGR2GRAY);

	Mat	element = getStructuringElement(MORPH_ELLIPSE, Size(8, 8));
	cv::morphologyEx(tmpImage1, tmpImage0, MORPH_GRADIENT, element);
	cv::threshold(tmpImage0, tmpImage0, 110, 255, THRESH_BINARY);
	cv::bitwise_and(tmpImage0, workMask2, tmpImage0);

	int beiji_count = 0;
	vector<vector<Point2i>> beiji_blob;
	find_beiji_Region(tmpImage0, 200, beiji_count, beiji_blob);
	tmpImage0.setTo(0);
	if (!CaculationUpdataCir(tmpImage0, beiji_blob, CurrentCenter, Currentradius)) return false;
	if (Currentradius > radius)
	{
		center = CurrentCenter;
		radius = Currentradius;
	}
	return true;
}

bool drawfringe(Mat &tmpMat, vector<vector<Point2i>> beiji_blob, int CountNumer, Point &Currentcenter, int &Currentradius, int &radius, Point &center, Point &StageCenterPoint, int&flag, Mat &src_Image, Mat &workMask2,bool addway)
{
	if (beiji_blob.size() == 0) return false;

	for (int i = 0; i < beiji_blob.size(); i++)
	{
		vector<Point2i> tmpPoint;
		tmpPoint = beiji_blob[i];
		for (int j = 0; j < tmpPoint.size(); j++)
		{
			Point dots;
			dots.x = tmpPoint[j].x;
			dots.y = tmpPoint[j].y;
			circle(tmpMat, dots, 0, 255, 1, 4);
		}
	}
	//imwrite("F:/TmpMat1.bmp", srcImage);

	//利用凸包检测+外接最小矩形-》得到最大内接圆
	vector<vector<Point2i> > allContour;
	vector<Point2i> Contour;
	vector<Point> points;//点值
	findContours(tmpMat, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < allContour.size(); i++)
	{
		Contour = allContour[i];
		for (int j = 0; j < Contour.size(); j++)
		{
			Point point;
			point.x = Contour[j].x;
			point.y = Contour[j].y;
			points.push_back(point);
		}
	}

	vector<int> hull;
	convexHull(Mat(points), hull, false, true);
	tmpMat.setTo(0);
	int hullcount = hull.size();                         //凸包的边数
	if (hullcount == 0) return false;
	if (hullcount > 0)
	{
		Point point0;
		point0 = points[(hull[hullcount - 1])];          //连接凸包边的坐标点
		for (int i = 0; i < hullcount; i++)
		{
			Point point1;
			point1 = points[(hull[i])];
			line(tmpMat, point0, point1, 255, 1, 8);   //将凸包点连接起来形成内边缘的外圈轨迹
			point0 = point1;
		}
	}

	//imwrite("F:/TmpMat2.bmp", srcImage);
	allContour.clear();
	Contour.clear();
	points.clear();
	findContours(tmpMat, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (allContour.size() != 1) return false;
	Contour = allContour[0];
	for (int j = 0; j < Contour.size(); j++)
	{
		Point point;
		point.x = Contour[j].x;
		point.y = Contour[j].y;
		points.push_back(point);
	}
	RotatedRect bouding = minAreaRect(Mat(points));
	Currentcenter = bouding.center;
	Currentradius = min(bouding.size.width, bouding.size.height) / 2;

	//半径偏离时取flag=1
	if (abs(bouding.center.x - StageCenterPoint.x)>20 && abs(bouding.center.y - StageCenterPoint.y)> 20  &&abs(Currentradius-radius)<radius/2 && CountNumer > 20)
	{
		flag = 1;
		return true;
	}
	if (Currentradius > radius && (abs(bouding.center.x - StageCenterPoint.x) + abs(bouding.center.y - StageCenterPoint.y)) < 35)
	{
		//maxCircleUpdata(src_Image, workMask2, radius, center);
		center = Currentcenter;
		if (!addway)
		{
			radius = radius + (min(bouding.size.width, bouding.size.height) / 2 - radius) / 2-2;

		}
		else
		{
			radius = radius + (min(bouding.size.width, bouding.size.height) / 2 - radius) / 2;

		}
		StageCenterPoint = Currentcenter;
	}
	return true;
}

bool drawfringe0(Mat &srcImage, vector<vector<Point2i>> beiji_blob, int CountNumer, Point &Currentcenter, int &Currentradius)
{
	if (beiji_blob.size() == 0) return false;

	for (int i = 0; i < beiji_blob.size(); i++)
	{
		vector<Point2i> tmpPoint;
		tmpPoint = beiji_blob[i];
		for (int j = 0; j < tmpPoint.size(); j++)
		{
			Point dots;
			dots.x = tmpPoint[j].x;
			dots.y = tmpPoint[j].y;
			circle(srcImage, dots, 0, 255, 1, 4);
		}
	}
	//imwrite("F:/TmpMat1.bmp", srcImage);

	//利用凸包检测+外接最小矩形-》得到最大内接圆
	vector<vector<Point2i> > allContour;
	vector<Point2i> Contour;
	vector<Point> points;//点值
	findContours(srcImage, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < allContour.size(); i++)
	{
		Contour = allContour[i];
		for (int j = 0; j < Contour.size(); j++)
		{
			Point point;
			point.x = Contour[j].x;
			point.y = Contour[j].y;
			points.push_back(point);
		}
	}

	vector<int> hull;
	convexHull(Mat(points), hull, false, true);
	srcImage.setTo(0);
	int hullcount = hull.size();                         //凸包的边数
	if (hullcount == 0) return false;
	if (hullcount > 0)
	{
		Point point0;
		point0 = points[(hull[hullcount - 1])];          //连接凸包边的坐标点
		for (int i = 0; i < hullcount; i++)
		{
			Point point1;
			point1 = points[(hull[i])];
			line(srcImage, point0, point1, 255, 1, 8);   //将凸包点连接起来形成内边缘的外圈轨迹
			point0 = point1;
		}
	}

	allContour.clear();
	Contour.clear();
	points.clear();
	findContours(srcImage, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (allContour.size() != 1) return false;
	Contour = allContour[0];
	for (int j = 0; j < Contour.size(); j++)
	{
		Point point;
		point.x = Contour[j].x;
		point.y = Contour[j].y;
		points.push_back(point);
	}
	RotatedRect bouding = minAreaRect(Mat(points));
	Currentcenter = bouding.center;
	Currentradius = min(bouding.size.width, bouding.size.height) / 2;
	return true;
}

bool backGroundMaskExtract(Mat &tmpMat, Mat& workMask1, Mat &workMask2, Point &center)
{
	if (tmpMat.empty()) return false;
	cv::threshold(tmpMat, tmpMat, 100, 255, THRESH_BINARY);
	//imwrite("e:/tmpMat2.bmp", tmpMat);
	Mat element0 = getStructuringElement(MORPH_RECT, Size(50, 50));     
	cv::morphologyEx(tmpMat, tmpMat, MORPH_CLOSE, element0);                        //腐蚀边界没有填充部分

	//imwrite("e:/tmpMat3.bmp", tmpMat);
	vector<vector<Point2i> > pallContour;
	vector<Point2i> pContour;
	vector<Point> points;
	findContours(tmpMat, pallContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (pallContour.size() == 0) return false;
	if (pallContour.size() != 1)
	{
		for (int i = 0; i < pallContour.size(); i++)
		{
			if (pallContour[i].size() > 500)
			{
				pContour = pallContour[i];
			}
		}
	}
	else
	{
		pContour = pallContour[0];
	}

	for (int i = 0; i < pContour.size(); i++)
	{
		Point point;
		point.x = pContour[i].x;
		point.y = pContour[i].y;
		circle(workMask1, point, 0, 255, 1, 8);
	}


	Point seed;
	seed.x = tmpMat.cols / 2;
	seed.y = tmpMat.rows / 2;
	Scalar newVal = Scalar(255);
	Rect ccomp;
	cv::floodFill(workMask1, seed, newVal, &ccomp, Scalar(20), Scalar(20), 4);
	Mat element1 = getStructuringElement(MORPH_ELLIPSE, Size(30, 30));                    //距离圆形工作盘外边缘15行之内的条纹不检
	cv::morphologyEx(workMask1, workMask2, MORPH_ERODE, element1);                        //腐蚀边界没有填充部分
	workMask1 = ~workMask1;
	center.x = ccomp.x + ccomp.width / 2;                                                 //得到粗略中心点
	center.y = ccomp.y + ccomp.height / 2;
	return true;
}

bool twentyClearImageExtract0(Point tmpCter, int tmpRdius, Point &center, int &radius, int &CountNumer, CenterPoint *ptrtmpCirPoint, Point &StageCenterPoint,bool addway)
{
	if (CountNumer == 0)
	{
		for (int i = 0; i < 20; i++)
		{
			ptrtmpCirPoint[i].x = 0;
			ptrtmpCirPoint[i].y = 0;
			ptrtmpCirPoint[i].radius = 0;
		}
	}
	if (CountNumer < 20)
	{
		if (abs(tmpCter.x - center.x) < 150 && abs(tmpCter.y - center.x) < 150)
		{
			ptrtmpCirPoint[CountNumer].x = tmpCter.x;
			ptrtmpCirPoint[CountNumer].y = tmpCter.y;
			ptrtmpCirPoint[CountNumer].radius = tmpRdius;
			CountNumer++;
			goto End;
		}
	}

	if (CountNumer == 20)                                    //20帧清晰条纹数据获得完毕
	{
		CenterPoint tmpCIR;
		CenterPoint tmpSortRadius[10];


		//冒泡排序
		//先从中心点的X进行排序
		for (int i = 0; i < 20 - 1; i++)
		{
			for (int j = 0; j < 20 - 1 - i; j++)
			{
				if (ptrtmpCirPoint[j].radius < ptrtmpCirPoint[j + 1].radius)
				{
					tmpCIR = ptrtmpCirPoint[j];
					ptrtmpCirPoint[j] = ptrtmpCirPoint[j + 1];
					ptrtmpCirPoint[j + 1] = tmpCIR;
				}
			}
		}
		for (int i = 0; i < 10; i++)
		{
			tmpSortRadius[i] = ptrtmpCirPoint[i];			 		
		}
		if (!addway)
		{
			radius = tmpSortRadius[0].radius - 2;                    //得到最大半径
		}
		else
		{
			radius = tmpSortRadius[0].radius;                    //得到最大半径

		}

		//再从中心点的Y进行排序
		for (int i = 0; i < 10 - 1; i++)
		{
			for (int j = 0; j < 10 - 1 - i; j++)
			{
				if (tmpSortRadius[j].x < tmpSortRadius[j + 1].x)
				{
					tmpCIR = tmpSortRadius[j];
					tmpSortRadius[j] = tmpSortRadius[j + 1];
					tmpSortRadius[j + 1] = tmpCIR;
				}
			}
		}
		center.x = (tmpSortRadius[4].x + tmpSortRadius[5].x) / 2;  //取中值得到中心点点的X坐标

		//再从中心点的Y进行排序
		for (int i = 0; i < 10 - 1; i++)
		{
			for (int j = 0; j < 10 - 1 - i; j++)
			{
				if (tmpSortRadius[j].y < tmpSortRadius[j + 1].y)
				{
					tmpCIR = tmpSortRadius[j];
					tmpSortRadius[j] = tmpSortRadius[j + 1];
					tmpSortRadius[j + 1] = tmpCIR;
				}
			}
		}
		center.y = (tmpSortRadius[4].y + tmpSortRadius[5].y) / 2;  //取中值得到中心点点的X坐标
		StageCenterPoint = center;
		CountNumer++;
		goto End;
	}

End:
	return true;
}

bool twentyClearImageExtract(Mat tmpMat, int& CountNumer, Point &center, int &radius, CenterPoint *ptrtmpCirPoint, Point&StageCenterPoint,bool addway)
{
	int beiji_count = 0;
	vector<vector<Point2i>> beiji_blob;
	find_beiji_Region(tmpMat, 200, beiji_count, beiji_blob);                                  //小于200的面积滤掉
	if (beiji_blob.size() >= 2 || (beiji_blob.size() == 1 && beiji_blob[0].size() > 10000))   //少于三条条纹的图片不捡
	{
		tmpMat.setTo(0);
		Point tmpCter;
		int tmpRdius;
		bool result = drawfringe0(tmpMat, beiji_blob, CountNumer, tmpCter, tmpRdius);
		if (result == false) return false;
		//20张条纹计算分析；
		twentyClearImageExtract0(tmpCter, tmpRdius, center, radius, CountNumer, ptrtmpCirPoint, StageCenterPoint,addway);
		return true;
	}
	else
	{
		return false;
	}
}

  void  onmouse(int event, int x, int y, int flags, void *lpParameter)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号    
{
	//CWnd hWnd = (CWnd)lpParameter;
	CinterferomeryDlg *pDlg = (CinterferomeryDlg*)lpParameter;
	//CWnd *hwnd;
	//hwnd = GetDlgItem(*pDlg,IDC_STATIC_PICTURE);
	HWND hwnd;
	hwnd = GetDlgItem(*pDlg, IDC_STATIC_PICTURE);
	//hwnd = pDlg->GetSafeHwnd();
	CPoint point;
	point.x = x;
	point.y = y;
	if (event == CV_EVENT_RBUTTONUP)
	{
		if ((x<(pDlg->showimgsizex / 4) && x>0 && y<(pDlg->showimgsizey/5) && y>0))
		{
			CMenu menu;
			menu.LoadMenu(IDR_MENU3);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			ClientToScreen(hwnd,&point);
			/*CWnd* pWndPopupOwner = this;
			while (pWndPopupOwner->GetStyle() & WS_CHILD)  pWndPopupOwner = pWndPopupOwner->GetParent();*/
			TaskDispose::RButtonPos = 0;
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pDlg);
			return;
		}
		else if ((x<(pDlg->showimgsizex)  && x>(pDlg->showimgsizex*3/4 ) && y<(pDlg->showimgsizey / 5) && y>0))
		{
			CMenu menu;
			menu.LoadMenu(IDR_MENU3);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			ClientToScreen(hwnd, &point);
			/*CWnd* pWndPopupOwner = this;
			while (pWndPopupOwner->GetStyle() & WS_CHILD)  pWndPopupOwner = pWndPopupOwner->GetParent();*/
			TaskDispose::RButtonPos = 1;
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pDlg);
			return;
		}
		else 
		{
			CMenu menu;
			menu.LoadMenu(IDR_MENU2);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
			CMenu* pPopup = menu.GetSubMenu(0);
			if (TaskDispose::aperModel == 1)
			{
				//pPopup->LoadMenuW(ID_AutoModel);
				pPopup->EnableMenuItem(ID_AutoModel, MF_DISABLED | MF_GRAYED);
				pPopup->EnableMenuItem(ID_MANUALSET, MF_DISABLED | MF_GRAYED);
				pPopup->EnableMenuItem(ID_APERSET, MF_DISABLED | MF_GRAYED);
				pPopup->CheckMenuItem(ID_MULTY, MF_CHECKED);
				pPopup->CheckMenuItem(ID_AutoModel, MF_CHECKED);
			}
			else if (TaskDispose::aperModel==0)
			{
				pPopup->EnableMenuItem(ID_AutoModel, MF_ENABLED);
				pPopup->EnableMenuItem(ID_MANUALSET, MF_ENABLED);
				pPopup->EnableMenuItem(ID_APERSET, MF_DISABLED | MF_GRAYED);
				pPopup->CheckMenuItem(ID_SIGLE, MF_CHECKED);
				pPopup->CheckMenuItem(ID_AutoModel, MF_CHECKED);
			}
			else if (TaskDispose::aperModel == 2)
			{
				pPopup->EnableMenuItem(ID_APERSET, MF_ENABLED);
				pPopup->CheckMenuItem(ID_MANUALSET, MF_CHECKED);
			}
			ASSERT(pPopup != NULL);
			ClientToScreen(hwnd,&point);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pDlg);
		}
	}
}

  void  onmmouse(int event, int x, int y, int flags, void *lpParameter)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号    
  {
	  //CWnd hWnd = (CWnd)lpParameter;
	  CinterferomeryDlg *pDlg = (CinterferomeryDlg*)lpParameter;
	  //CWnd *hwnd;
	  //hwnd = GetDlgItem(*pDlg,IDC_STATIC_PICTURE);
	  HWND hwnd;
	  hwnd = GetDlgItem(*pDlg, IDC_STATIC_PICTURE);
	  //hwnd = pDlg->GetSafeHwnd();
	  CPoint point;
	  point.x = x;
	  point.y = y;
	  if (event == CV_EVENT_RBUTTONUP)
	  {
		  if ((x<700 && x>200 && y<100 && y>0))
		  {
			  CMenu menu;
			  menu.LoadMenu(IDR_MENU3);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
			  CMenu* pPopup = menu.GetSubMenu(0);
			  ASSERT(pPopup != NULL);
			  ClientToScreen(hwnd, &point);
			  /*CWnd* pWndPopupOwner = this;
			  while (pWndPopupOwner->GetStyle() & WS_CHILD)  pWndPopupOwner = pWndPopupOwner->GetParent();*/
			  TaskDispose::RButtonPos = 0;
			  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pDlg);
			  return;
		  }
		  else if ((x<720 && x>520 && y<100 && y>0))
		  {
			  CMenu menu;
			  menu.LoadMenu(IDR_MENU3);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
			  CMenu* pPopup = menu.GetSubMenu(0);
			  ASSERT(pPopup != NULL);
			  ClientToScreen(hwnd, &point);
			  /*CWnd* pWndPopupOwner = this;
			  while (pWndPopupOwner->GetStyle() & WS_CHILD)  pWndPopupOwner = pWndPopupOwner->GetParent();*/
			  TaskDispose::RButtonPos = 1;
			  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pDlg);
			  return;
		  }
		  else
		  {
			  CMenu menu;
			  menu.LoadMenu(IDR_MENU2);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
			  CMenu* pPopup = menu.GetSubMenu(0);
			  if (TaskDispose::aperModel == 1)
			  {
				  //pPopup->LoadMenuW(ID_AutoModel);
				  pPopup->EnableMenuItem(ID_AutoModel, MF_DISABLED | MF_GRAYED);
				  pPopup->EnableMenuItem(ID_MANUALSET, MF_DISABLED | MF_GRAYED);
				  

			  }
			  else if (TaskDispose::aperModel == 0)
			  {
				  pPopup->EnableMenuItem(ID_AutoModel, MF_ENABLED);
				  pPopup->EnableMenuItem(ID_MANUALSET, MF_ENABLED);
				  //pPopup->CheckMenuRadioItem(ID_SIGLE, ID_MULTY, ID_SIGLE, MF_BYCOMMAND);
			  }
			  ASSERT(pPopup != NULL);
			  ClientToScreen(hwnd, &point);
			  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pDlg);
		  }
	  }
  }


void threadGetCapture(LPVOID lpParameter)
{
	CinterferomeryDlg *pDlg = (CinterferomeryDlg*)lpParameter;
	cv::VideoCapture vcap("7.avi");
	//VideoCapture vcap(0);
	//TaskDispose::vcap.open(0);
	vcap.set(CV_CAP_PROP_FPS, 20);
	//setMouseCallback("MyMainShowWnd", onmmouse, (void*)&srcframe);
	//setMouseCallback("MyMainShowWnd", onmouse, (void*)&lpParameter);
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 1, 8);
	int index = 0;//视频帧计数，超过20未找到轮廓清零，去除手指移动等干扰的影响
	int showcircleindex = 0;//发现轮廓后，绘制轮廓时间计数
	Mat frame1(540, 720, CV_8UC3, cv::Scalar::all(0));//帧间差分用，保留当前帧
	Mat frame2(540, 720, CV_8UC3, cv::Scalar::all(0));//帧间差分用，保留上一帧
	Mat PTellipse(540, 720, CV_8UC1, cv::Scalar::all(0));
	std::vector< std::vector< cv::Point> > automulticontoursrebuild;
	std::vector< std::vector< cv::Point> > automulticontoursrebuildcut;
	ellipse(PTellipse, cvPoint(378, 278), cvSize(210, 224), 0, 0, 360, Scalar(1), -1);
	Mat grayframe1;
	Mat grayframe2;
	Mat subframe(540, 720, CV_8UC1, cv::Scalar::all(0));
	Mat addframe(540, 720, CV_8UC1, cv::Scalar::all(0));
	Mat darkframe(540, 720, CV_8UC1, cv::Scalar::all(0));
	Mat contourframe;
	bool havegetcontour = false;
	bool coneff = false;
	bool drawcircle = false;
	int coneffcount = 0;
	int widstripecount = 0;
	Point getconcenterpoint;
	int losetimes = 0;
	int   getradius = 0;
	Mat elementgetcontour = getStructuringElement(MORPH_OPEN, Size(5, 5));
	Mat element3 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element5 = getStructuringElement(MORPH_RECT, Size(5, 5));

	Mat readframe(480, 640, CV_8UC1, cv::Scalar::all(0));
	//HANDLE hMutex;
	//hMutex = CreateMutex(NULL, FALSE, NULL);

	///////////////////自动单件模式///////////////////////
	//新加入的变量：
	//新加入的变量：
	bool SingelOrMore = false;
	int MaxCircleRadius = 0;
	int delaypicshowCir = 0;
	CenterPoint NumCenterPoint[20];
	Point StageCenterPoint;
	int flag = 0;
	int a = 0;
	int aa = 0;
	vector<int> aaa;
	int aaaa = 0;
	int num = 0;
	int backgroundCircle = 1;                                        //1为有背景圆  0为没有背景圆


	//核心变量分配内存
	Point center;                                                 //内切圆的中心
	center.x = 0;
	center.y = 0;
	int radius = 0;                                               //内切圆的半径  跳出while循环即视频读取完毕之后 得到最后最佳的内切圆
	CenterPoint tmpCirPoint[20];                                  //分配开始20张清晰条纹内存大小
	CenterPoint* ptrtmpCirPoint = tmpCirPoint;
	Mat src_Image, tmpMat;
	Mat workMask1(540, 720, CV_8UC1, Scalar(0));                  //放置工作盘掩模图
	Mat workMask2(540, 720, CV_8UC1, Scalar(0));                  //放置工作盘掩模图的腐蚀图（去掉梯度检测后的工作盘最外圈的圆环）
	Mat tmpMat3(540,720, CV_8UC1, Scalar::all(0));


	//逻辑变量：
	int startCount = 0;                                           //startCount帧之后开始处理 避免前期没必要处理的帧
	int  startCaulationCenter = 0;                                // startCaulationCenter计算有背景圆的掩模图
	int CountNumer = 0;
	StageCenterPoint.x = 0;
	StageCenterPoint.y = 0;
	bool ret = true;
	bool addway = true;
	int pp = 0;
	int ppp = 0;


	///////////////////自动单件模式///////////////////////
	Mat tempPtMap(512, 512, CV_8UC1, cv::Scalar::all(0));
	bool result = true;


	while (true)
	{
		waitKey(10);
		if (!stop)
		{
			//WaitForSingleObject(hMutex, INFINITE);

			if (!vcap.isOpened())
			{
				vcap.open(0);
				vcap.set(CV_CAP_PROP_FPS, 20);
			}

			vcap >> readframe;
			if (!readframe.data) 
				break;
			//resize(readframe, srcframe, Size(720, 540), 0, 0, INTER_LANCZOS4);
			cv::resize(readframe, srcframe, Size(720, 540), 0, 0, INTER_LINEAR);
			//TaskDispose::TaskSrcFrame = srcframe;
			srcframe.copyTo(TaskDispose::TaskSrcFrame);
			/////////////////////////////手动模式/////////////////////////////////////////////
			if (TaskDispose::aperModel == 2)
			{
				SingelOrMore = true;
				pDlg->DrawResult(srcframe);
				/*Mat includePtimg(srcframe.size(), CV_8UC1, cv::Scalar::all(0));
				Mat PtROI = includePtimg(Rect(TaskDispose::origin.x, TaskDispose::origin.y, 512, 512));
				Rect rect = Rect(TaskDispose::origin.x, TaskDispose::origin.y, 512, 512);
				TaskDispose::PtMap.convertTo(PtROI, includePtimg.type(), 1, 0);
				includePtimg = includePtimg * 255;
				Mat colorPtimg;
				cvtColor(includePtimg, colorPtimg, CV_GRAY2BGR);*/

				//vector<Mat> channels;	//vector<Mat>： 可以理解为存放Mat类型的容器（数组） 
				//split(colorPtimg, channels);  //对原图像进行通道分离，即把一个3通道图像转换成为3个单通道图像channels[0],channels[1] ,channels[2]
				//vector<Mat> mbgr(3);	//创建类型为Mat，数组长度为3的变量mbgr
				//Mat hideChannel(colorPtimg.size(), CV_8UC1, Scalar(0));//需要隐藏的通道。尺寸与srcImage相同，单通道黑色图像。
				//Mat imageB(colorPtimg.size(), CV_8UC3);	//创建尺寸与srcImage相同，三通道图像imageB
				//mbgr[0] = hideChannel;
				//mbgr[1] = channels[0];
				//mbgr[2] = channels[1];
				//merge(mbgr, imageB);
				//addWeighted(imageB, 0.1, srcframe, 1, 0.0, srcframe);
				circle(srcframe, Point(TaskDispose::x, TaskDispose::y), TaskDispose::radius, Scalar(255, 255, 255), 1);
				cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
				cv::imshow("MyMainShowWnd", showsrcframe);
				if (TaskDispose::staticanalysis)
				{
					if (index % 30 == 2 && losetimes < 5)
					{
						srcframe.copyTo(waitforprocframe);
						//SendMessage(pDlg->m_hWnd, WM_STATICANAYSIS, FALSE, FALSE);
						if (pDlg->StaticAnaysis())
						{
							TaskDispose::staticanalysis = false;
							losetimes = 0;
							index = 0;
						}
						else
						{
							losetimes++;
						}
					}
					if (losetimes == 5)
					{
						TaskDispose::staticanalysis = false;
						losetimes = 0;
						index = 0;
					}
				}
				index++;
			}
			/////////////////////////////手动模式/////////////////////////////////////////////


			/////////////////////////////自动单件模式/////////////////////////////////////////////
			else if (TaskDispose::aperModel == 0)
			{
				if (SingelOrMore)
				{
					center.x = 0;
					center.y = 0;
					radius = 0;
					workMask1.setTo(0);
					workMask2.setTo(0);
					tmpMat3.setTo(0);
					MaxCircleRadius = 0;
					delaypicshowCir = 0;
					flag = 0;
					a = 0;
					aa = 0;
					aaa.clear();
					aaaa = 0;
					num = 0;
					backgroundCircle = 1;
					startCount = 0;                                           //startCount帧之后开始处理 避免前期没必要处理的帧
					startCaulationCenter = 0;                                // startCaulationCenter计算有背景圆的掩模图
					CountNumer = 0;
					StageCenterPoint.x = 0;
					StageCenterPoint.y = 0;
					ret = true;
					SingelOrMore = false;
					addway = true;
					ppp = 0;
					pp = 0;

				}
				startCount++;
				if (startCount < 10) continue;                                                                //100帧之后开始处理 避免前期没必要处理的帧
				if (startCount > 99999999) startCount = 101;

				//【1】转单通道图进行处理
				cv::cvtColor(srcframe, tmpMat, CV_RGB2GRAY);
				//imwrite("F:/TmpMat.bmp", tmpMat);
				Mat currentImg = tmpMat.clone();
				//保存当前帧原图

				if (ppp == 0)
				{

					Point p1, p2;
					p1.x = 0;
					p1.y = 2;
					p2.x = currentImg.cols;
					p2.y = currentImg.rows;
					cv::line(tmpMat3, p1, p2, Scalar(255), 2, 4);
					ppp++;
				}

				if (startCaulationCenter == 0)
				{
					Mat	element3 = getStructuringElement(MORPH_ELLIPSE, Size(8, 8));
					cv::morphologyEx(currentImg, tmpMat, MORPH_GRADIENT, element3);                                     //梯度检测
					cv::threshold(tmpMat, tmpMat, 110, 255, THRESH_BINARY);
					//imwrite("e:/workMat123.bmp", tmpMat);

					int beiji_count = 0;
					vector<vector<Point2i>> beiji_blob;
					find_beiji_Region0(tmpMat, 200, beiji_count, beiji_blob);
					if (beiji_blob.size() >= 1)
					{
						//【2】得到背景圆掩模图和粗略中心点（once）	

						ret = backGroundMaskExtract(currentImg, workMask1, workMask2, center);
						if (!ret) continue;
						//imwrite("e:/workMat1.bmp", workMask1);
						//imwrite("e:/workMat2.bmp", workMask2);
						cv::Rect bouding = cv::boundingRect(workMask2);
						MaxCircleRadius = min(bouding.width, bouding.height) / 2;
						startCaulationCenter++;

					}
					else
					{
						continue;
					}
				}

				//【3】梯度检测提取干涉条纹特征
				if (abs(MaxCircleRadius - radius) < 15)
				{
					addway = false;
				}

				if (!addway)   //没有背景圆
				{
					Mat	element = getStructuringElement(MORPH_ELLIPSE, Size(8, 8));
					cv::morphologyEx(currentImg, tmpMat, MORPH_GRADIENT, element);                                     //梯度检测
					cv::threshold(tmpMat, tmpMat, 110, 255, THRESH_BINARY);                                        //阈值化提取特征
				}
				else
				{
					cv::addWeighted(currentImg, 1, workMask1, -1, 0, tmpMat);                                      //当前帧背景黑化
					Mat	element = getStructuringElement(MORPH_ELLIPSE, Size(8, 8));
					cv::morphologyEx(tmpMat, tmpMat, MORPH_GRADIENT, element);                                     //梯度检测
					cv::threshold(tmpMat, tmpMat, 110, 255, THRESH_BINARY);                                        //阈值化提取特征
					cv::bitwise_and(tmpMat, workMask2, tmpMat);                                                    //得到背景圆范围内特征
				}

				if (!addway)   //没有背景圆
				{
					Mat	element = getStructuringElement(MORPH_RECT, Size(50, 50));
					Mat tmpMat2;
					cv::morphologyEx(workMask2, tmpMat2, MORPH_ERODE, element);
					cv::bitwise_and(tmpMat, tmpMat2, tmpMat2);  
					addWeighted(tmpMat2, 1, tmpMat3, -1,0,tmpMat2);
					imwrite("e:/workMat123.bmp", tmpMat2);
					//得到背景圆范围内特征
					int beiji_count1 = 0;
					vector<vector<Point2i>> beiji_blob1;
					find_beiji_Region(tmpMat2, 200, beiji_count1, beiji_blob1);
					if (beiji_blob1.size() < 2)        //条纹清晰的条数少于两条的图片不捡
					{
						pDlg->DrawResult(srcframe);
						cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
						cv::imshow("MyMainShowWnd", showsrcframe);
						waitKey(10);
						continue;
					}
				}
					

				//【4】blob分析获取最清晰20个条纹
				int beiji_count = 0;
				vector<vector<Point2i>> beiji_blob;
				find_beiji_Region(tmpMat, 200, beiji_count, beiji_blob);                                       //小于200的面积滤掉
				if (CountNumer <= 20)
				{
					ret = twentyClearImageExtract(tmpMat, CountNumer, center, radius, tmpCirPoint, StageCenterPoint, addway);
					pDlg->DrawResult(srcframe);
					cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
					cv::imshow("MyMainShowWnd", showsrcframe);
					waitKey(10);                                                      //显示当前帧
					continue;
				}

				//【5】实时内接圆更新
				if (beiji_blob.size() >= 1 || (beiji_blob.size() == 1 && beiji_blob[0].size() > 10000))        //少于条纹太少的图片不捡
				{
					tmpMat.setTo(0);
					Point tmpCter;
					int tmpRdius;
					flag = 0;
					ret = drawfringe(tmpMat, beiji_blob, CountNumer, tmpCter, tmpRdius, radius, center, StageCenterPoint, flag, src_Image, workMask2,addway);
					if (ret == false)
					{
						pDlg->DrawResult(srcframe);
						cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
						cv::imshow("MyMainShowWnd", showsrcframe);
						waitKey(10);                                   //显示当前帧
						continue;
					}

					if (flag == 1 && num == 0)                         //flag为1  开始出现偏离当前转态的位置出现
					{
						a = 1;                                         //此时a置1   开始计数 
						num = 1;                                       //计数开关
						aaa.clear();
						pDlg->DrawResult(srcframe);
						cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
						cv::imshow("MyMainShowWnd", showsrcframe);
						waitKey(10);
						continue;
					}

					if (a == 1 && num == 1)
					{
						aaa.push_back(flag);                           //开始记录接下来10个里面有几个偏离之前的位置
						if (aaa.size() == 10)
						{
							for (int i = 0; i < aaa.size(); i++)
							{
								if (aaa[i] > 0)
								{
									aaaa++;
								}
							}

							if (aaaa < 5)
							{
								a = 0;
								circle(srcframe, center, cvRound(radius), Scalar(0, 0, 255), 2, LINE_AA);
								pDlg->DrawResult(srcframe);
								cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
								cv::imshow("MyMainShowWnd", showsrcframe);
								waitKey(10);
								num = 0;
								continue;

							}
							else
							{
								CountNumer = 0;                           //连续10张中出现有5张中心点异变情况开始更新新的中心点
								a = 0;
								pDlg->DrawResult(srcframe);
								cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
								cv::imshow("MyMainShowWnd", showsrcframe);
								waitKey(10);
								num = 0;
								continue;
							}

						}
						else
						{
							if (flag == 1)
							{
								pDlg->DrawResult(srcframe);
								cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
								cv::imshow("MyMainShowWnd", showsrcframe);
								waitKey(10);
								continue;
							}
							else
							{
								circle(srcframe, center, cvRound(radius), Scalar(0, 0, 255), 2, LINE_AA);
								pDlg->DrawResult(srcframe);
								cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
								cv::imshow("MyMainShowWnd", showsrcframe);
								waitKey(10);
								continue;
							}
						}
					}

					//【6】判断条纹出现显示内接圆
					if (radius != 0)                                   //20个清晰条纹处理完毕后，一旦有条纹出现显示圆圈
					{
						delaypicshowCir = 1;
						circle(srcframe, center, cvRound(radius), Scalar(0, 0, 255), 2, LINE_AA);
						//imwrite("e:/srcframe.jpg", srcframe);
						pDlg->DrawResult(srcframe);
						cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
						cv::imshow("MyMainShowWnd", showsrcframe);
						waitKey(10);
						continue;
					}
					else
					{
						pDlg->DrawResult(srcframe);
						cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
						cv::imshow("MyMainShowWnd", showsrcframe);
						waitKey(10);
						continue;
					}
				}
				else
				{
					if (delaypicshowCir != 0 && delaypicshowCir < 10)
					{
						circle(srcframe, center, cvRound(radius), Scalar(0, 0, 255), 2, LINE_AA);
						pDlg->DrawResult(srcframe);
						cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
						cv::imshow("MyMainShowWnd", showsrcframe);
						waitKey(10);
						delaypicshowCir++;
						continue;
					}
					else
					{
						pDlg->DrawResult(srcframe);
						cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
						cv::imshow("MyMainShowWnd", showsrcframe);
						waitKey(10);
						delaypicshowCir = 0;
						continue;
					}
				}

				/*srcframe.copyTo(waitforprocframe);
				Mat tempPtMap(512, 512, CV_8UC1, cv::Scalar::all(0));
				Point PtMapCenter;
				PtMapCenter.x = getconcenterpoint.x - TaskDispose::origin.x;
				PtMapCenter.y = getconcenterpoint.y - TaskDispose::origin.y;
				circle(tempPtMap, PtMapCenter, getradius, Scalar(1), -1);
				tempPtMap.copyTo(TaskDispose::PtMap);
				if (!pDlg->StaticAnaysis())
		resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
		imshow("MyMainShowWnd", showsrcframe);*/

			}
			/////////////////////////////自动单件模式/////////////////////////////////////////////


			/////////////////////////////自动多件模式/////////////////////////////////////////////
			else if (TaskDispose::aperModel == 1)
			{
			    SingelOrMore = true;
				frame1.copyTo(frame2);
				srcframe.copyTo(frame1);
				cvtColor(frame1, grayframe1, CV_RGB2GRAY);
				cvtColor(frame2, grayframe2, CV_RGB2GRAY);
				//multiply(PTellipse, grayframe1, grayframe1, 1, -1);
				//multiply(PTellipse, grayframe2, grayframe2, 1, -1);
				std::vector< std::vector< cv::Point> > contours;
				if (!havegetcontour)
				{
					if (index % 50 == 0 && !havegetcontour)
					{
						darkframe.copyTo(addframe);
					}
					if (index > 5)
					{
						addWeighted(grayframe1, 1, grayframe2, -1, 0, subframe);
						adaptiveThreshold(subframe, subframe, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 45, -25);
						addWeighted(subframe, 1, addframe, 1, 0, addframe);
					}
					erode(addframe, addframe, element3);//腐蚀，去除孤立亮点
					dilate(addframe, addframe, element3);//膨胀，去除孤立黑点

					addframe.copyTo(contourframe);
					cv::findContours(contourframe, contours, cv::noArray(), cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
					int num_50 = 0;
					for (int i = 0; i < contours.size(); i++)
					{
						if (contours[i].size()>50)
						{
							num_50++;
						}
					}
					/*
						int avagesize = 0;
						for (int i = 0; i < contours.size(); i++)
						{
							avagesize += contours[i].size();
							if (contours[i].size() < 30)
								num_30++;
						}
						if (contours.size() > 0)
						{
							avagesize = avagesize / contours.size();
						}*/
						//if (avagesize > 30 && num_30 < 10 && contours.size() < 50)
					if (num_50 > 2)

					{
						//widstripecount++;
						//if (widstripecount > 30)
						//{
						havegetcontour = true;
						darkframe.copyTo(addframe);
						//}
					}
				}
				else {

					addWeighted(grayframe1, 1, grayframe2, -1, 0, subframe);
					adaptiveThreshold(subframe, subframe, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 45, -25);
					addWeighted(subframe, 1, addframe, 1, 0, addframe);
					erode(addframe, addframe, element3); //腐蚀，去除孤立亮点
					dilate(addframe, addframe, element3);//膨胀，去除孤立黑点
					dilate(addframe, addframe, element3);//膨胀，去除孤立黑点
					erode(addframe, addframe, element3); //腐蚀，去除孤立亮点
					if (showcircleindex == 150)
					{
						int num_30 = 0;
						int averagesize = 0;
						Point contourcenter;
						int radius;
						pDlg->m_listBox.ResetContent();
						addframe.copyTo(contourframe);
						cv::findContours(contourframe, contours, cv::noArray(), cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
						for (int i = 0; i < contours.size(); i++)
						{
							pDlg->GetContourCenter(contours[i], contourcenter, radius);
							int dis = sqrt(pow((TaskDispose::MultiCtrCen.x - contourcenter.x), 2) + pow((TaskDispose::MultiCtrCen.y - contourcenter.y), 2));
							if (contours[i].size()>50 && ((dis + radius) < TaskDispose::MultiCtrRadius))
							{
								automulticontoursrebuild.push_back(contours[i]);
								averagesize += contours[i].size();
							}
							if (contours[i].size() < 30)
							{
								num_30++;
							}
						}
						if (num_30 > 15)
						{
							showcircleindex = 0;
							havegetcontour = false;
							darkframe.copyTo(addframe);
							automulticontoursrebuild.clear();
							automulticontoursrebuildcut.clear();
							continue;
						}
						if (automulticontoursrebuild.size() > 0)
						{
							averagesize = averagesize / automulticontoursrebuild.size();
						}

						for (int i = 0; i < automulticontoursrebuild.size(); i++)
						{

							if (automulticontoursrebuild[i].size()>averagesize*0.8)
							{
								automulticontoursrebuildcut.push_back(automulticontoursrebuild[i]);
							}
						}
					}
					if (showcircleindex == 153)
					{
						Point contourcenter;
						int radius;
						//srcframe.copyTo(waitforprocframe);
						int textindex = 0;
						for (int i = 0; i < automulticontoursrebuildcut.size(); i++)
						{
							pDlg->GetContourCenter(automulticontoursrebuildcut[i], contourcenter, radius);
							radius -= 3;
							int dis = sqrt(pow((TaskDispose::MultiCtrCen.x - contourcenter.x), 2) + pow((TaskDispose::MultiCtrCen.y - contourcenter.y), 2));
							if (radius > 8 && contourcenter.x > radius&&contourcenter.y > radius && ((dis + radius) < TaskDispose::MultiCtrRadius))
							{
								//circle(srcframe, contourcenter, radius, Scalar(0, 0, 255), 2);
								Mat cutimg = srcframe(Rect(contourcenter.x - radius, contourcenter.y - radius, radius * 2, radius * 2));
								Mat cutimg128;
								cv::resize(cutimg, cutimg128, Size(128, 128), 0, 0, INTER_LINEAR);
								cutimg128.copyTo(waitforprocframe);
								/*Mat tempPtMap(512, 512, CV_8UC1, cv::Scalar::all(0));
								circle(tempPtMap, contourcenter, radius, Scalar(1), -1);
								tempPtMap.copyTo(TaskDispose::PtMap);*/
								Mat tempPtMap(radius * 2, radius * 2, CV_8UC1, cv::Scalar::all(0));
								circle(tempPtMap, Point(radius, radius), radius, Scalar(1), -1);
								Mat tempPtMap128;
								cv::resize(tempPtMap, tempPtMap128, Size(128, 128), 0, 0, INTER_LINEAR);
								tempPtMap128.copyTo(TaskDispose::PtMap);
								//if (i==0)
								//{
									//imshow("src", cutimg128);
									//imshow("pt", tempPtMap64*255);
								//}
								if (pDlg->StaticAnaysis())
								{
									CString strnum;
									strnum.Format(_T("%d# "), textindex);
									CString str;
									//str.Format(_T("%d# PV %5.2f POWER %5.2f"), textindex, PV, POWER);
									str = pDlg->DrawResult(srcframe);
									str = strnum + str;
									//char t[256];
									//sprintf_s(t, "%d# PV %5.2f POWER %5.2f",i, PV,POWER);
									//str = t;
									//strPV += "Wave";
									//pDlg->m_listBox.AddString(_T("1# PV 0.054fr POWER 0.5fr"));
									pDlg->m_listBox.AddString(str);

								}
								textindex++;
							}

						}
					}
					if (showcircleindex == 300)
					{
						showcircleindex = 0;
						havegetcontour = false;
						darkframe.copyTo(addframe);
						automulticontoursrebuild.clear();
						automulticontoursrebuildcut.clear();
					}
					showcircleindex++;
					if (showcircleindex > 150)
					{
						string str;
						Point contourcenter;
						int radius;
						int textindex = 0;
						for (int i = 0; i < automulticontoursrebuildcut.size(); i++)
						{
							str = to_string(textindex);
							pDlg->GetContourCenter(automulticontoursrebuildcut[i], contourcenter, radius);
							radius -= 3;
							int dis = sqrt(pow((TaskDispose::MultiCtrCen.x - contourcenter.x), 2) + pow((TaskDispose::MultiCtrCen.y - contourcenter.y), 2));
							if (radius > 8 && ((dis + radius) < TaskDispose::MultiCtrRadius))
							{
								circle(srcframe, contourcenter, radius, Scalar(0, 0, 255), 2);
								contourcenter.x -= 13;
								contourcenter.y += 10;
								cv::putText(srcframe, str, contourcenter, FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255), 2, 6, 0);
								textindex++;
							}

						}
					}

				}

				index++;
				//circle(srcframe, TaskDispose::MultiCtrCen, TaskDispose::MultiCtrRadius, Scalar(0, 255, 0), 2);
				cv::resize(srcframe, showsrcframe, Size(pDlg->showimgsizex, pDlg->showimgsizey), 0, 0, INTER_LINEAR);
				cv::imshow("MyMainShowWnd", showsrcframe);
				//imshow("subframe", subframe);
				//imshow("addframe", addframe);
			}
		}
		/////////////////////////////自动多件模式/////////////////////////////////////////////
		//ReleaseMutex(hMutex);
	}
	vcap.release();
}

CinterferomeryDlg::CinterferomeryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INTERFEROMERY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_zernCoef = new float[36];
	Sv = new float[36];
	for (int i = 0; i < 36; i++) m_zernCoef[i] = 0.0;
	for (int i = 0; i < 4; i++) m_zern4[i] = 0.0;
	for (int i = 0; i < 36; i++) Sv[i] = 0.0;
}
//CinterferomeryDlg::~CinterferomeryDlg()
//{
//     delete[] m_zernCoef;
//}

void CinterferomeryDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (lpMeasureItemStruct->CtlType == ODT_MENU)//如果类型是菜单
		newMenu.MeasureItem(lpMeasureItemStruct);//调用CNewMenu类的MeasureItem成员函数
	else
		CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);

}
//void CinterferomeryDlg::OnDrawItem()
void CinterferomeryDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl==ID_START)
	{
		lpDrawItemStruct->rcItem.left += 30;
	}
	if (lpDrawItemStruct->CtlType == ODT_MENU)
		//ClientToScreen(this, &lpDrawItemStruct->rcItem);
		newMenu.DrawItem(lpDrawItemStruct);
	else
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
void CinterferomeryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listBox);
	//  DDX_Control(pDX, IDC_MFCVSLISTBOX1, m_cvslistbox);
}

BEGIN_MESSAGE_MAP(CinterferomeryDlg, CDialogEx)
	//ON_MESSAGE(WM_STATICANAYSIS, StaticAnaysis)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_MANUALSET, &CinterferomeryDlg::OnManualset)
ON_WM_RBUTTONDOWN()
ON_COMMAND(ID_START, &CinterferomeryDlg::OnStart)
ON_COMMAND(ID_AutoModel, &CinterferomeryDlg::OnAutomodel)
//ON_MESSAGE(WM_MEASUREITEM, OnMeasureItem)
//ON_MESSAGE(WM_DRAWITEM, OnDrawItem)
ON_WM_CREATE()
ON_COMMAND(ID_STATICANALYSIS, &CinterferomeryDlg::OnStaticAnalysis)
ON_COMMAND(ID_SIGLE, &CinterferomeryDlg::OnSigle)
ON_COMMAND(ID_MULTY, &CinterferomeryDlg::OnMulty)
ON_COMMAND(ID_PV_PV, &CinterferomeryDlg::OnPvPv)
ON_COMMAND(ID_PV_PVRMS, &CinterferomeryDlg::OnPvPvrms)
ON_COMMAND(ID_PV_POWER, &CinterferomeryDlg::OnPower)
ON_COMMAND(ID_PV_N, &CinterferomeryDlg::OnN)
ON_COMMAND(ID_PV_DELTAN, &CinterferomeryDlg::OnDeltan)
ON_COMMAND(ID_PV_aperture, &CinterferomeryDlg::Onaperture)
ON_COMMAND(ID_PV_Asia, &CinterferomeryDlg::OnAsia)
ON_COMMAND(ID_PV_FR, &CinterferomeryDlg::OnPvFr)
ON_COMMAND(ID_PV_WAVE, &CinterferomeryDlg::OnPvWave)
ON_COMMAND(ID_PV_um, &CinterferomeryDlg::OnPvum)
ON_COMMAND(ID_PV_NM, &CinterferomeryDlg::OnPvNm)
ON_COMMAND(ID_PVRMS_FR, &CinterferomeryDlg::OnPvrmsFr)
ON_COMMAND(ID_PVRMS_WAVE, &CinterferomeryDlg::OnPvrmsWave)
ON_COMMAND(ID_PVRMS_um, &CinterferomeryDlg::OnPvrmsum)
ON_COMMAND(ID_PVRMS_NM, &CinterferomeryDlg::OnPvrmsNm)
ON_COMMAND(ID_POWER_FR, &CinterferomeryDlg::OnPowerFr)
ON_COMMAND(ID_POWER_WAVE, &CinterferomeryDlg::OnPowerWave)
ON_COMMAND(ID_POWER_um, &CinterferomeryDlg::OnPowerum)
ON_COMMAND(ID_POWER_NM, &CinterferomeryDlg::OnPowerNm)
ON_WM_RBUTTONUP()
ON_COMMAND(ID_32779, &CinterferomeryDlg::On32779)
ON_COMMAND(ID_32778, &CinterferomeryDlg::On32778)
ON_COMMAND(ID_32777, &CinterferomeryDlg::On32777)
ON_COMMAND(ID_32790, &CinterferomeryDlg::On32790)
ON_COMMAND(ID_EXCEL, &CinterferomeryDlg::OnExcel)
ON_COMMAND(ID_32838, &CinterferomeryDlg::On32838)
ON_WM_CTLCOLOR()
ON_WM_SIZE()
ON_COMMAND(ID_APERSET, &CinterferomeryDlg::OnAperset)
ON_COMMAND(ID_32843, &CinterferomeryDlg::On32843)
ON_COMMAND(ID_32844, &CinterferomeryDlg::On32844)
END_MESSAGE_MAP()
// CinterferomeryDlg 消息处理程序
void CinterferomeryDlg::OnNcPaint()
{
	CDialog::OnNcPaint();
	CDC* pDC = GetWindowDC();
	int x = GetSystemMetrics(SM_CXSIZE) + GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXDLGFRAME);
	int y = GetSystemMetrics(SM_CYDLGFRAME);
	CRect CapRct;
	GetWindowRect(&CapRct);
	int y1 = GetSystemMetrics(SM_CYICON) - GetSystemMetrics(SM_CYDLGFRAME) - GetSystemMetrics(SM_CYBORDER);
	int x1 = CapRct.Width() - GetSystemMetrics(SM_CXSIZE) - GetSystemMetrics(SM_CXBORDER) - GetSystemMetrics(SM_CXDLGFRAME);

	CapRct.left = x;
	CapRct.top = y;
	CapRct.right = x1;
	CapRct.bottom = y1;

	//pDC->FillSolidRect(&CapRct,RGB(0,0,0));
	CFont* pCurFont = GetFont();
	LOGFONT lf;
	pCurFont->GetLogFont(&lf);
	lf.lfItalic = FALSE;
	lf.lfWeight = FW_BOLD;
	lf.lfHeight = 24;
	//strcpy(lf.lfFaceName, "Arial");

	CFont capfont;
	capfont.CreateFontIndirect(&lf);
	pCurFont = pDC->SelectObject(&capfont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
	//pDC->DrawText(_T("激光干涉测量软件"), &CapRct, DT_SINGLELINE | DT_CENTER | DT_VCENTER); //最好定义一个m_strCaption类成员

	ReleaseDC(pDC);
}
BOOL CinterferomeryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_hbrush = CreateSolidBrush(RGB(0, 0, 0));
	SetBackgroundColor(RGB(0, 0, 0));
	showimgsizex = 720;
	showimgsizey = 567;
	ShowWindow(SW_SHOWMAXIMIZED);
	
	
	//newMenu.LoadMenu(IDR_MENU1);
	//newMenu.ChangeMenuItem(&newMenu);
	//SetMenu(&newMenu);
	//newMenu.ChangeMenuItem(newMenu.GetSubMenu(0));

	//MENUINFO   menuinfo;
	//menuinfo.cbSize = sizeof(MENUINFO);
	//menuinfo.fMask =  MIM_BACKGROUND;
	//menuinfo.hbrBack = ::CreateSolidBrush(RGB(0, 0, 0));
	//
	//CMenu*  pMenu = GetMenu();
	//::SetMenuInfo(pMenu->m_hMenu, &menuinfo);
	//DrawMenuBar();

	
	//CBrush m_brush;
	//::MENUINFO lpcmi; //：：必须写不然无法识别
	//m_brush.CreateSolidBrush(RGB(0, 0, 0));//你的颜色
	//memset(&lpcmi, 0, sizeof(::LPCMENUINFO));
	//lpcmi.cbSize = sizeof(MENUINFO);
	//lpcmi.fMask = MIM_BACKGROUND;
	//lpcmi.hbrBack = (HBRUSH)m_brush.operator HBRUSH();
	//::SetMenuInfo(GetMenu()->m_hMenu, &lpcmi);
	
	//DWORD dwStyle = GetStyle();//获取旧样式  
	//DWORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//dwNewStyle &= dwStyle;//按位与将旧样式去掉  
	//SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//设置成新的样式  
	//DWORD dwExStyle = GetExStyle();//获取旧扩展样式  
	//DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	//dwNewExStyle &= dwExStyle;//按位与将旧扩展样式去掉  
	//SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewExStyle);//设置新的扩展样式  
	//SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//告诉windows：我的样式改变了，窗口位置和大小保持原来不变！ 

	LOGFONT logfont = { 0 };
	::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
	logfont.lfHeight = 26;
	logfont.lfWeight = 700;
	afxGlobalData.SetMenuFont(&logfont, true);
	//LOGFONT logfont;
	//lstrcpy(logfont.lfFaceName, _T("楷体_GB2312"));
	//logfont.lfWeight = 700;
	//logfont.lfWidth = 20;
	//logfont.lfHeight = 50;
	//logfont.lfEscapement = 0;
	//logfont.lfUnderline = FALSE;
	//logfont.lfItalic = FALSE;
	//logfont.lfStrikeOut = FALSE;
	//logfont.lfCharSet = GB2312_CHARSET; //以上参数好像一个都不能少 
	//m_wndMenuBar.SetMenuFont(&logfont);
	/*LOGFONT m_lf;
	memset(&m_lf, 0, sizeof(LOGFONT));
	m_lf.lfHeight = 26;
	_tcsncpy_s(m_lf.lfFaceName, LF_FACESIZE, _T("Arial"), 27);*/
	cv::namedWindow("MyMainShowWnd", WINDOW_AUTOSIZE);
	//cv::resizeWindow("MyMainShowWnd", 720, 576);
	HWND hWnd = (HWND)cvGetWindowHandle("MyMainShowWnd");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_PICTURE)->m_hWnd);
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_STATIC_PICTURE); //获取控件指针，IDC_BUTTON1为控件ID号
	//pWnd->SetWindowPos(NULL, 20, 20, 720, 576, SWP_NOZORDER);
	::ShowWindow(hParent, SW_HIDE);
	setMouseCallback("MyMainShowWnd", onmouse, this);

	f = new CFont;
	f->CreateFont(14, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_BOLD, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("宋体")); // lpszFac 
	//f->CreateFontIndirect(&logfont);
	 //m_wndMenuBar.SetMenuFont(&logfont, TRUE);
	 //m_wndMenuBar.SetFont(f, 1);
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO: 在此添加额外的初始化代码
	//m_StatBar = new CStatusBarCtrl;
	//RECT m_Rect;
	//GetClientRect(&m_Rect); //获取对话框的矩形区域
	//m_Rect.top = m_Rect.bottom - 20; //设置状态栏的矩形区域
	//m_StatBar->Create(WS_BORDER | WS_VISIBLE | CBRS_BOTTOM, m_Rect, this, 3);
	//int nParts[4] = { 200, 400, 550,-1 }; //分割尺寸
	//m_StatBar->SetParts(4, nParts); //分割状态栏
	//m_StatBar->SetText(_T("自动选择孔径模式"), 0, 0); //第一个分栏加入"这是第一个指示器"

	HThreadGetCapture = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadGetCapture, this, 0, 0);

	//m_StatBar->SetFont(f);
	//m_StatBar->SetText(_T("第二个指示器"), 1, 0); //以下类似
	//m_StatBar->SetText(_T("第三个指示器"), 2, 0);
	//m_StatBar->SetText(_T("第四个指示器"), 3, 0);
	//CFont* pCurFont = GetFont();
	//LOGFONT lf;
	//pCurFont->GetLogFont(&lf);
	//lf.lfItalic = TRUE;
	//lf.lfWeight = FW_BOLD;
	//lf.lfHeight = 18;
	//newMenu.LoadMenu(IDR_MENU1);
	//SetMenu(&newMenu);
	//newMenu.ChangeMenuItem(&newMenu);//(&newMenu);newMenu.GetSubMenu(0)
	//::EnumChildWindows(m_hWnd, CinterferomeryDlg::EnumChildProc, (LPARAM)&f);
	//AperSet dlg;
	//dlg.CreateEffectPtMap();
	//SetWindowPos(NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
	
	/*HDC hDesktopDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	int xScrn = GetDeviceCaps(hDesktopDC, HORZRES);
	int yScrn = GetDeviceCaps(hDesktopDC, VERTRES);
	MoveWindow(0, 0, xScrn, yScrn);*/
	
	//CRect rect;
	//GetClientRect(&rect);     //取客户区大小  
	//old.x = rect.right - rect.left;
	//old.y = rect.bottom - rect.top;
	//CRect clientrect;
	//GetClientRect(clientrect);
	////CRect  rect;
	//GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(rect);
	//int x = clientrect.Size().cx*0.15;
	//int y = clientrect.Size().cy*0.15;
	//GetDlgItem(IDC_STATIC_PICTURE)->SetWindowPos(0, x, y, rect.Size().cx, rect.Size().cy, NULL);
	CRect clientrect;
	GetClientRect(clientrect);
	GetDlgItem(IDC_STATIC_PICTURE)->MoveWindow(clientrect, TRUE);
	showimgsizex = clientrect.Size().cx;
	showimgsizey = clientrect.Size().cy;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
BOOL CALLBACK CinterferomeryDlg::EnumChildProc(HWND hwndChild, LPARAM lParam)
{
	CFont *pFont = (CFont*)lParam;
	CWnd *pWnd = CWnd::FromHandle(hwndChild);
	pWnd->SetFont(pFont);
	return TRUE;
}
void CinterferomeryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标//显示。
HCURSOR CinterferomeryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CinterferomeryDlg::DrawPicToHDC(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}


void CinterferomeryDlg::OnManualset()
{
	// TODO: 在此添加命令处理程序代码
	//m_StatBar->SetText(_T("单片手动选择孔径模式"), 0, 0);
	TaskDispose::aperModel = 2;
	TaskDispose::length = 128;
	//TaskDispose::stopAperWnd = false;
	/*CRect clientrect;
	GetClientRect(clientrect);
	CRect  rect;
	GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(rect);
	int x = clientrect.Size().cx*0.15;
	int y = clientrect.Size().cy*0.15;
	GetDlgItem(IDC_STATIC_PICTURE)->SetWindowPos(0, x, y, rect.Size().cx, rect.Size().cy, NULL);*/
	CRect clientrect;
	GetClientRect(clientrect);
	GetDlgItem(IDC_STATIC_PICTURE)->MoveWindow(clientrect, TRUE);
	showimgsizex = clientrect.Size().cx;
	showimgsizey = clientrect.Size().cy;
	
}
Mat CinterferomeryDlg::CreateEffectPtMap()
{
	m_aperType =TaskDispose::aperType;
	Point center;
	center.x = TaskDispose::x;
	center.y = TaskDispose::y;
	m_aperRadius = TaskDispose::radius;
	m_aperHeight = TaskDispose::height;
	m_aperWidth = TaskDispose::width;
	m_aperRin = TaskDispose::rin;
	m_aperRout = TaskDispose::rout;
	Mat PtMap(512, 512, CV_8UC1, cv::Scalar::all(0));

	Rect rect;
	rect.width = m_aperWidth;
	rect.height = m_aperHeight;
	if ((center.x-rect.width/2)<0||(center.y-rect.height/2)<0|| (center.x + rect.width / 2)>512 || (center.y + rect.height/2)>512)
	{
		MessageBox(_T("Aper size is setted wrong! Please reset!"));
		return PtMap;
	}
	rect.x = center.x - rect.width / 2;
	rect.y = center.y - rect.height / 2;
	if ((center.x-m_aperRadius)<0|| (center.y - m_aperRadius)<0 || (center.x + m_aperRadius)>512 || (center.y + m_aperRadius)>512)
	{
		MessageBox(_T("Aper size is setted wrong! Please reset!"));
		return PtMap;
	}
	if ((center.x - m_aperRout) < 0|| (center.y - m_aperRout) < 0||(center.x + m_aperRout) >512|| (center.y + m_aperRout) >512)
	{
		MessageBox(_T("Aper size is setted wrong! Please reset!"));
		return PtMap;
	}
	switch (m_aperType)
	{
	case 0:
		circle(PtMap, center, m_aperRadius, Scalar(1),-1);
		break;
	case 1:
		PtMap(rect).setTo(1);
		break;
	case 2:
		circle(PtMap, center, m_aperRout, Scalar(1),-1);
		circle(PtMap, center, m_aperRin, Scalar(0),-1);
		break;
	}



	return PtMap;
}
Mat CinterferomeryDlg::BWinterferogram()
{
	//if (TaskDispose::aperModel == 1)
	//{
		Mat bwimg(128, 128, CV_8UC1, cv::Scalar::all(0));
		double sum = 0.0;
		int	num = 0;
		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				if (ucharPtMap[i][j])
				{
					sum += (double)Image[i][j];
					num++;
				}
		int		v;
		if (num > 0)
			v = (int)(sum / (double)num);
		else return bwimg;

		if (v < 120) v -= 30;

		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				if (ucharPtMap[i][j])
				{
					if (Image[i][j] > v) bwimg.at<uchar>(i, j) = WHITE;
					else  bwimg.at<uchar>(i, j) = BLACK;
				}
				else
					bwimg.at<uchar>(i, j) = BLACK;

		return bwimg;
	//}
	/*else
	{
		Mat bwimg(512, 512, CV_8UC1, cv::Scalar::all(0));
		double sum = 0.0;
		int	num = 0;
		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				if (ucharPtMap[i][j])
				{
					sum += (double)Image[i][j];
					num++;
				}
		int		v;
		if (num > 0)
			v = (int)(sum / (double)num);
		else return bwimg;

		if (v < 120) v -= 30;

		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				if (ucharPtMap[i][j])
				{
					if (Image[i][j] > v) bwimg.at<uchar>(i, j) = WHITE;
					else  bwimg.at<uchar>(i, j) = BLACK;
				}
				else
					bwimg.at<uchar>(i, j) = BLACK;

		return bwimg;
	}*/
}

//对二值化图像进行预处理
void	CinterferomeryDlg::BWPreprocess(Mat& bwimg, Mat ptimg)
{
	//去除孤立白点
	for (int i = 1; i < TaskDispose::length - 1; i++)
		for (int j = 1; j < TaskDispose::length - 1; j++)
			if (ptimg.at<uchar>(i, j) && bwimg.at<uchar>(i, j) == WHITE)
				if (((ptimg.at<uchar>(i - 1, j) && bwimg.at<uchar>(i-1, j) == BLACK) || !ptimg.at<uchar>(i - 1, j)) &&
					((ptimg.at<uchar>(i + 1, j) && bwimg.at<uchar>(i+1, j) == BLACK) || !ptimg.at<uchar>(i + 1, j)) &&
					((ptimg.at<uchar>(i, j - 1) && bwimg.at<uchar>(i, j-1) == BLACK) || !ptimg.at<uchar>(i, j - 1)) &&
					((ptimg.at<uchar>(i, j + 1) && bwimg.at<uchar>(i, j+1) == BLACK) || !ptimg.at<uchar>(i, j + 1)) &&
					((ptimg.at<uchar>(i - 1, j - 1) && bwimg.at<uchar>(i-1, j-1) == BLACK) || !ptimg.at<uchar>(i - 1, j - 1)) &&
					((ptimg.at<uchar>(i + 1, j + 1) && bwimg.at<uchar>(i+1, j+1) == BLACK) || !ptimg.at<uchar>(i + 1, j + 1)) &&
					((ptimg.at<uchar>(i - 1, j + 1) && bwimg.at<uchar>(i-1, j+1) == BLACK) || !ptimg.at<uchar>(i - 1, j + 1)) &&
					((ptimg.at<uchar>(i + 1, j - 1) && bwimg.at<uchar>(i+1, j-1) == BLACK) || !ptimg.at<uchar>(i + 1, j - 1)))
					bwimg.at<uchar>(i, j) = BLACK;

	//去除孤立黑点
	for (int i = 1; i < TaskDispose::length - 1; i++)
		for (int j = 1; j < TaskDispose::length - 1; j++)
			if (ptimg.at<uchar>(i, j) && bwimg.at<uchar>(i, j) == BLACK)
				if (((ptimg.at<uchar>(i - 1, j) && bwimg.at<uchar>(i-1, j) == WHITE) || !ptimg.at<uchar>(i - 1, j)) &&
					((ptimg.at<uchar>(i + 1, j) && bwimg.at<uchar>(i+1, j) == WHITE) || !ptimg.at<uchar>(i + 1, j)) &&
					((ptimg.at<uchar>(i, j - 1) && bwimg.at<uchar>(i, j-1) == WHITE) || !ptimg.at<uchar>(i, j - 1)) &&
					((ptimg.at<uchar>(i, j + 1) && bwimg.at<uchar>(i, j+1) == WHITE) || !ptimg.at<uchar>(i, j + 1)) &&
					((ptimg.at<uchar>(i - 1, j - 1) && bwimg.at<uchar>(i-1, j-1) == WHITE) || !ptimg.at<uchar>(i - 1, j - 1)) &&
					((ptimg.at<uchar>(i + 1, j + 1) && bwimg.at<uchar>(i+1, j+1) == WHITE) || !ptimg.at<uchar>(i + 1, j + 1)) &&
					((ptimg.at<uchar>(i - 1, j + 1) && bwimg.at<uchar>(i-1, j+1) == WHITE) || !ptimg.at<uchar>(i - 1, j + 1)) &&
					((ptimg.at<uchar>(i + 1, j - 1) && bwimg.at<uchar>(i+1, j-1) == WHITE) || !ptimg.at<uchar>(i + 1, j - 1)))
					bwimg.at<uchar>(i, j) = WHITE;

	// Move noise point
	//unsigned char(*m_BWMapTemp)[LENGTH] = new unsigned char[LENGTH][LENGTH];
	//if (!m_BWMapTemp) {
	//	AfxMessageBox("Not enough memory");
	//	return;
	//}
	Mat m_BWMapTemp=bwimg;
	//for (int i = 0; i < LENGTH; i++)
	//	for (int j = 0; j < LENGTH; j++)
	//		m_BWMapTemp[i][j] = m_BWMap[i][j];

	//统计黑白点数
	int	whitenum = 0;
	int	blacknum = 0;
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (ptimg.at<uchar>(i, j))
				if (bwimg.at<uchar>(i, j) == WHITE) whitenum++;
				else blacknum++;

				double rate = (double)blacknum / (double)(whitenum + blacknum);

				int times;
				if (rate > 0.42) times = 2;
				else times = 0;
				times = 0;
				for (int k = 0; k < times; k++)
				{
					for (int i = 1; i < TaskDispose::length - 1; i++)
						for (int j = 1; j < TaskDispose::length - 1; j++)
							if (ptimg.at<uchar>(i, j) && bwimg.at<uchar>(i, j) == BLACK)
								if (((ptimg.at<uchar>(i - 1, j) && bwimg.at<uchar>(i-1, j) == BLACK) || !ptimg.at<uchar>(i - 1, j)) &&
									((ptimg.at<uchar>(i + 1, j) && bwimg.at<uchar>(i+1, j) == BLACK) || !ptimg.at<uchar>(i + 1, j)) &&
									((ptimg.at<uchar>(i, j - 1) && bwimg.at<uchar>(i, j-1) == BLACK) || !ptimg.at<uchar>(i, j - 1)) &&
									((ptimg.at<uchar>(i, j + 1) && bwimg.at<uchar>(i, j+1) == BLACK) || !ptimg.at<uchar>(i, j + 1)) &&
									((ptimg.at<uchar>(i - 1, j - 1) && bwimg.at<uchar>(i-1, j-1) == BLACK) || !ptimg.at<uchar>(i - 1, j - 1)) &&
									((ptimg.at<uchar>(i + 1, j + 1) && bwimg.at<uchar>(i+1, j+1) == BLACK) || !ptimg.at<uchar>(i + 1, j + 1)) &&
									((ptimg.at<uchar>(i - 1, j + 1) && bwimg.at<uchar>(i-1, j+1) == BLACK) || !ptimg.at<uchar>(i - 1, j + 1)) &&
									((ptimg.at<uchar>(i + 1, j - 1) && bwimg.at<uchar>(i+1, j-1) == BLACK) || !ptimg.at<uchar>(i + 1, j - 1)))
									m_BWMapTemp.at<uchar>(i, j) = BLACK;
								else m_BWMapTemp.at<uchar>(i, j) = WHITE;

								//for (int i = 0; i < LENGTH; i++)
								//	for (int j = 0; j < LENGTH; j++)
								//		bwimg.at<uchar>(i, j) = m_BWMapTemp[i][j];
								bwimg = m_BWMapTemp;
				}
				//delete[] m_BWMapTemp;
}
void CinterferomeryDlg::cvShiftDFT(Mat src)
{
	int cx = src.cols / 2;
	int cy = src.rows / 2;
	Mat dst;
	src.copyTo(dst);
	//dst(Rect(cx - 1, cy - 1, cx, cy)) = src(Rect(0, 0, cx, cy));
	//dst(Rect(0, 0, cx, cy)) = src(Rect(cx - 1, cy - 1, cx, cy));
	//dst(Rect(0, cy - 1, cx, cy)) = src(Rect(cx - 1, 0, cx, cy));
	//dst(Rect(cx - 1, 0, cx, cy)) = src(Rect(0, cy - 1, cx, cy));

	src(Rect(0, 0, cx, cy)).copyTo(dst(Rect(cx , cy , cx, cy)));
	src(Rect(cx , cy , cx, cy)).copyTo(dst(Rect(0, 0, cx, cy)));
	src(Rect(cx , 0, cx, cy)).copyTo(dst(Rect(0, cy , cx, cy)));
	src(Rect(0, cy , cx, cy)).copyTo(dst(Rect(cx , 0, cx, cy)));
	dst.copyTo(src);
}
void CinterferomeryDlg::showtime(DWORD start, DWORD end)
{
	DWORD usetime = end - start;
	CString str;
	str.Format(_T("%d"), usetime);
	MessageBox(str);
}
//void CinterferomeryDlg::NoiseRemoveInterferogram(Mat& input, Mat& output)
//{
//	/************************频域低通滤波处理****************************/
//	cv::Mat dftInput1, dftImage1, inverseDFT, mergeimg;
//	input.convertTo(dftInput1, CV_32F);
//	cv::dft(dftInput1, dftImage1, cv::DFT_COMPLEX_OUTPUT);    // Applying DFT
//	cvShiftDFT(dftImage1);
//	vector<Mat> channels;
//	split(dftImage1, channels);//分离通道
//	Mat noise_PtMap(512, 512, CV_32F, cv::Scalar::all(1));
//	circle(noise_PtMap, Point(256, 256),15, Scalar(0), -1);
//	multiply(channels.at(0), noise_PtMap, channels.at(0), 1, -1);
//	multiply(channels.at(1), noise_PtMap, channels.at(1), 1, -1);
//	merge(channels, mergeimg);
//	cvShiftDFT(mergeimg);
//	cv::idft(mergeimg, inverseDFT, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT); // Applying IDFT
//	inverseDFT.convertTo(output, CV_8U);
//	/************************频域低通滤波处理****************************/
//}
bool CinterferomeryDlg::FindUnlabedPt(unsigned char(*temdata)[LENGTH], int& x, int& y)
{
	BOOL	found = FALSE;
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (ucharPtMap[i][j] &&
				(temdata[i][j] == BLACK || temdata[i][j] == WHITE))
			{
				x = i;
				y = j;
				found = TRUE;
				break;
			}
		return found;
}
bool CinterferomeryDlg::LableDown(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 0; i < TaskDispose::length - 1; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (temdata[i][j] == num &&
				temdata[i+1][j] == bw && ucharPtMap[i+1][j])
			{
				temdata[i+1][j] = num;
				TotalPts++;
				bFlag = TRUE;
			}
		return bFlag;
}
bool CinterferomeryDlg::LableLeft(unsigned char(*temdata)[LENGTH],  unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
		for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 1; j < TaskDispose::length; j++)
			if (temdata[i][j] == num &&
				temdata[i][j - 1] == bw && ucharPtMap[i][j-1])
			{
				temdata[i][j - 1] = num;
				TotalPts++;
				bFlag = TRUE;
			}
	return bFlag;
}
bool CinterferomeryDlg::LableRight(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (temdata[i][j] == num &&
				temdata[i][j + 1] == bw && ucharPtMap[i][j+1])
			{
				temdata[i][j + 1] = num;
				TotalPts++;
				bFlag = TRUE;
			}
	return bFlag;
}
bool CinterferomeryDlg::LableUp(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 1; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (temdata[i][j] == num &&
				temdata[i-1][j] == bw && ucharPtMap[i-1][j])
			{
				temdata[i-1][j] = num;
				TotalPts++;
				bFlag = TRUE;
			}
	return bFlag;
}
bool CinterferomeryDlg::LableUpLeft(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 1; i < TaskDispose::length; i++)
		for (int j = 1; j < TaskDispose::length; j++)
			if (temdata[i][j] == num &&
				temdata[i-1][j-1] == bw && ucharPtMap[i-1][j-1])
			{
				temdata[i-1][j-1] = num;
				TotalPts++;
				bFlag = TRUE;
			}
	return bFlag;
}
bool CinterferomeryDlg::LableUpRight(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 1; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length -1; j++)
			if (temdata[i][j] == num &&
				temdata[i-1][j+1] == bw &&ucharPtMap[i-1][j+1])
			{
				temdata[i-1][j+1] = num;
				TotalPts++;
				bFlag = TRUE;
			}
	return bFlag;
}
bool CinterferomeryDlg::LableDownLeft(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 0; i < TaskDispose::length -1; i++)
		for (int j = 1; j < TaskDispose::length; j++)
			if (temdata[i][j] == num &&
				temdata[i+1][j-1] == bw && ucharPtMap[i+1][j-1])
			{
				temdata[i+1][j-1] = num;
				TotalPts++;
				bFlag = TRUE;
			}
	return bFlag;
}
bool CinterferomeryDlg::LableDownRight(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 0; i < TaskDispose::length -1; i++)
		for (int j = 0; j < TaskDispose::length - 1; j++)
			if (temdata[i][j]==num &&
				temdata[i+1][j+1] == bw && ucharPtMap[i+1][j+1])
			{
				temdata[i+1][j+1] = num;
				TotalPts++;
				bFlag = TRUE;
			}
	return bFlag;
}
void CinterferomeryDlg::LablePt(unsigned char(*tempdata)[LENGTH] ,int x, int y,unsigned char bw,int num, int& TotalPts)
{
	//标号确定的算法
	tempdata[x][y] = num;
	TotalPts = 1;
	/*while (LableLeft(tempdata, bw, num, TotalPts) ||
		LableRight(tempdata, bw, num, TotalPts) ||
		LableUp(tempdata, bw, num, TotalPts) ||
		LableDown(tempdata, bw, num, TotalPts) ||
		LableUpLeft(tempdata, bw, num, TotalPts) ||
		LableUpRight(tempdata, bw, num, TotalPts) ||
		LableDownLeft(tempdata, bw, num, TotalPts) ||
		LableDownRight(tempdata, bw, num, TotalPts));*/
	while (Lable(tempdata, bw, num, TotalPts));
	

}
bool CinterferomeryDlg::Lable(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts)
{
	bool	bFlag = FALSE;
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (temdata[i][j] == num)
			{
				if(((i+1)<TaskDispose::length)&&((j+1)<TaskDispose::length)&&temdata[i + 1][j + 1] == bw && ucharPtMap[i + 1][j + 1])
				{
					temdata[i + 1][j + 1] = num;
					TotalPts++;
					bFlag = TRUE;
				}
				else if (((i + 1)<TaskDispose::length)  && temdata[i + 1][j] == bw && ucharPtMap[i + 1][j])
				{
					temdata[i + 1][j] = num;
					TotalPts++;
					bFlag = TRUE;
				}
				else if (((i + 1) < TaskDispose::length) &&( j >0) && temdata[i + 1][j - 1] == bw && ucharPtMap[i + 1][j - 1])
				{
					temdata[i + 1][j - 1] = num;
					TotalPts++;
					bFlag = TRUE;
				}
				else if (((j + 1) < TaskDispose::length) && temdata[i][j + 1] == bw && ucharPtMap[i][j + 1])
				{
					temdata[i][j + 1] = num;
					TotalPts++;
					bFlag = TRUE;
				}
				else if ((j>0) && temdata[i][j - 1] == bw && ucharPtMap[i][j - 1])
				{
					temdata[i][j -1] = num;
					TotalPts++;
					bFlag = TRUE;
				}
				else if ((i>0) && temdata[i -1][j] == bw && ucharPtMap[i - 1][j])
				{
					temdata[i - 1][j] = num;
					TotalPts++;
					bFlag = TRUE;
				}
				else if ((i>0)&& (j > 0) && temdata[i - 1][j - 1] == bw && ucharPtMap[i - 1][j - 1])
				{
					temdata[i - 1][j - 1] = num;
					TotalPts++;
					bFlag = TRUE;
				}
				else if ((i > 0) && ((j + 1) < TaskDispose::length) && temdata[i - 1][j + 1] == bw && ucharPtMap[i - 1][j + 1])
				{
					temdata[i - 1][j + 1] = num;
					TotalPts++;
					bFlag = TRUE;
				}
			}




	/*if (temdata[i][j] == num &&
		temdata[i + 1][j + 1] == bw && ucharPtMap[i + 1][j + 1])
	{
		temdata[i + 1][j + 1] = num;
		TotalPts++;
		bFlag = TRUE;
	}*/
	return bFlag;
}
void CinterferomeryDlg::LableInterferogram(unsigned char(*temdata)[LENGTH], BLOCKINF block[], int& blocknum)
{
	int	num = 1;
	int	x, y;
	
	
	while (FindUnlabedPt(temdata, x, y) && num < BLOCKNUMMAX)
	{
		block[num].bw = temdata[x][y];
		LablePt(temdata, x, y, block[num].bw, num, block[num].TotalPts);
		num++;
	}
	
	
	blocknum = num - 1;
}
void CinterferomeryDlg::InitialBlockInf(BLOCKINF block[], int blockNum)
{
	for (int i = 1; i <= blockNum; i++)
	{
		block[i].bDefine = FALSE;
		block[i].bDeleted = FALSE;
		for (int j = 0; j < MAXBLOCKNUM; j++)
		{
			block[i].NeighbourBlock[j] = 0;
			block[i].NeighbourLength[j] = 0;
		}
		block[i].NeighbourNum = 0;
	}
}
void CinterferomeryDlg::FindNeighbour(unsigned char(*image)[LENGTH], BLOCKINF block[], int number)
{
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (image[i][j] == number)
			{
				if (j > 1 && image[i][j - 1] != number)
					if (ucharPtMap[i ][j - 1])
						InsertNeighbourInf(block, number, image[i][j - 1]);

				if (j > 1 && i > 1 && image[i - 1][j - 1] != number)
					if (ucharPtMap[i - 1][j - 1])
						InsertNeighbourInf(block, number, image[i - 1][j - 1]);

				if (i > 1 && image[i - 1][j] != number)
					if (ucharPtMap[i - 1][j])
						InsertNeighbourInf(block, number, image[i - 1][j]);

				if (i > 1 && j < TaskDispose::length - 1 &&
					image[i - 1][j + 1] != number)
					if (ucharPtMap[i - 1][j + 1])
						InsertNeighbourInf(block, number, image[i - 1][j + 1]);

				if (j < TaskDispose::length - 1 &&
					image[i][j + 1] != number)
					if (ucharPtMap[i][j + 1])
						InsertNeighbourInf(block, number, image[i][j + 1]);

				if (i < TaskDispose::length - 1 && j < TaskDispose::length - 1 &&
					image[i + 1][j + 1] != number)
					if (ucharPtMap[i + 1][j + 1])
						InsertNeighbourInf(block, number, image[i + 1][j + 1]);

				if (i < TaskDispose::length - 1 &&
					image[i + 1][j] != number)
					if (ucharPtMap[i + 1][j])
						InsertNeighbourInf(block, number, image[i + 1][j]);

				if (i < TaskDispose::length - 1 && j > 1 &&
					image[i + 1][j - 1] != number)
					if (ucharPtMap[i + 1][j - 1])
						InsertNeighbourInf(block, number, image[i + 1][j - 1]);

			}
}
//插入相邻标记
void CinterferomeryDlg::InsertNeighbourInf(BLOCKINF block[], int number, int Neigh)
{
	BOOL	bFound = FALSE;
	int i;
	for (i = 0; i < block[number].NeighbourNum; i++)
	{
		if (block[number].NeighbourBlock[i] == Neigh)
		{
			bFound = TRUE;
			block[number].NeighbourLength[i]++;
			break;
		}
	}

		if (!bFound && block[number].NeighbourNum < MAXBLOCKNUM)
		{
			block[number].NeighbourBlock[i] = Neigh;
			block[number].NeighbourLength[i]++;
			block[number].NeighbourNum++;
		}

}
//删除非合理的块
void CinterferomeryDlg::DeleteUnwantedBlock(unsigned char(*image)[LENGTH],BLOCKINF block[],int	blockNum)
{
	for (int i = 1; i <= blockNum; i++)
		if (block[i].TotalPts < REASONABLEBLOCKNUM && block[i].NeighbourNum == 1)
		{
			block[i].bDeleted = TRUE;
			int	index = block[i].NeighbourBlock[0];
			for (int j = 0; j < TaskDispose::length; j++)
				for (int k = 0; k < TaskDispose::length; k++)
					if (ucharPtMap[j][k] && image[j][k] == i)
					{
						if (block[i].bw == WHITE)
							m_BWMap.at<uchar>(j,k) = BLACK;
						else
							m_BWMap.at<uchar>(j, k) = WHITE;
						image[j][k] = index;
					}
			for (int j = 0; j < block[index].NeighbourNum; j++)
				if (block[index].NeighbourBlock[j] == i)
				{
					for (int k = j + 1; k < block[index].NeighbourNum; k++)
						block[index].NeighbourBlock[k - 1] = block[index].NeighbourBlock[k];
					block[index].NeighbourNum--;
					break;
				}
		}
	if (TaskDispose::aperModel == 1)
	{
		Sleep(200);
	}
}
//判断最大邻居数是否超过两个以上
bool CinterferomeryDlg::IsMaxNeighbourNumGreaterThanTwo(BLOCKINF	block[],int	blockNum)
{
	bool	bV = FALSE;
	for (int i = 1; i <= blockNum; i++)
		if (block[i].NeighbourNum > 2)
			bV = TRUE;
	return bV;
}
//针对相邻数超过2的情况，对二值化图象的适当区域作细化处理
void CinterferomeryDlg::ReProcessBWMap(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum)
{
	for (int i = 1; i <= blockNum; i++)
		if (block[i].NeighbourNum > 2)
		{
			PartialThin(image, block, i);
		}
}
//对二值化图象的部分区域作细化处理
void CinterferomeryDlg::PartialThin(unsigned char(*image)[LENGTH],BLOCKINF	block[],int num)
{
	unsigned char(*Temp)[LENGTH] = new unsigned char[LENGTH][LENGTH];
	unsigned char(*Temp1)[LENGTH] = new unsigned char[LENGTH][LENGTH];
	unsigned char	InP, OutP;
	if (block[num].bw == WHITE)
	{
		InP = WHITE;		OutP = BLACK;
	}
	else
	{
		InP = BLACK;		OutP = WHITE;
	}

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (image[i][j] == num)
				Temp[i][j] = Temp1[i][j] = InP;
			else
				Temp[i][j] = Temp1[i][j] = OutP;

	for (int k = 0; k < 2; k++)
	{
		for (int i = 1; i < TaskDispose::length - 1; i++)
			for (int j = 1; j < TaskDispose::length - 1; j++)
				if (ucharPtMap[i][j] && Temp[i][j] == InP)
					if (((ucharPtMap[i - 1][j] && Temp[i - 1][j] == InP) || !ucharPtMap[i - 1][j]) &&
						((ucharPtMap[i + 1][j] && Temp[i + 1][j] == InP) || !ucharPtMap[i + 1][j]) &&
						((ucharPtMap[i][j - 1] && Temp[i][j - 1] == InP) || !ucharPtMap[i + 1][j - 1]) &&
						((ucharPtMap[i][j + 1] && Temp[i][j + 1] == InP) || !ucharPtMap[i][j + 1]) &&
						((ucharPtMap[i - 1][j - 1] && Temp[i - 1][j - 1] == InP) || !ucharPtMap[i - 1][j - 1]) &&
						((ucharPtMap[i + 1][j + 1] && Temp[i + 1][j + 1] == InP) || !ucharPtMap[i + 1][j + 1]) &&
						((ucharPtMap[i - 1][j + 1] && Temp[i - 1][j + 1] == InP) || !ucharPtMap[i - 1][j + 1]) &&
						((ucharPtMap[i + 1][j - 1] && Temp[i + 1][j - 1] == InP) || !ucharPtMap[i + 1][j - 1]))
						Temp1[i][j] = InP;
					else Temp1[i][j] = OutP;

					for (int i = 0; i < TaskDispose::length; i++)
						for (int j = 0; j < TaskDispose::length; j++)
							Temp[i][j] = Temp1[i][j];
	}

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (Temp[i][j] == OutP && image[i][j] == num)
				m_BWMap.at<uchar>(i,j) = OutP;

	delete[] Temp;
	delete[] Temp1;
}
//干涉图的级次处理
bool CinterferomeryDlg::OrderInterferogram(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum)
{
	//	AfxMessageBox("Define orders");
	//确定第一个有效块的级次
	for (int i = 1; i <= blockNum; i++)
		if (!block[i].bDeleted)
		{
			block[i].order = 0;
			block[i].bDefine = TRUE;
			break;
		}
	int	cycle = 0;
	int cyclenum;
	//if (TaskDispose::aperModel==1)
	//{
		cyclenum = 200;
	/*}
	else
	{
		cyclenum = 500;
	}*/
	while (!OrderFinished(block, blockNum) && cycle < cyclenum)//500
	{
		for (int i = 1; i <= blockNum; i++)
			if (block[i].bDefine && !block[i].bDeleted)
			{
				if (block[i].NeighbourNum == 1 &&
					!block[block[i].NeighbourBlock[0]].bDefine)
				{
					block[block[i].NeighbourBlock[0]].order =
						block[i].order + 1;
					block[block[i].NeighbourBlock[0]].bDefine = TRUE;
				}
				if (block[i].NeighbourNum == 2)
				{
					if (!block[block[i].NeighbourBlock[0]].bDefine &&
						!block[block[i].NeighbourBlock[1]].bDefine)
					{
						block[block[i].NeighbourBlock[0]].order =
							block[i].order + 1;
						block[block[i].NeighbourBlock[0]].bDefine = TRUE;
						block[block[i].NeighbourBlock[1]].order =
							block[i].order - 1;
						block[block[i].NeighbourBlock[1]].bDefine = TRUE;
					}
					if (!block[block[i].NeighbourBlock[0]].bDefine &&
						block[block[i].NeighbourBlock[1]].bDefine)
					{
						if (block[block[i].NeighbourBlock[1]].order > block[i].order)
							block[block[i].NeighbourBlock[0]].order =
							block[i].order - 1;
						else
							block[block[i].NeighbourBlock[0]].order =
							block[i].order + 1;
						block[block[i].NeighbourBlock[0]].bDefine = TRUE;
					}
					if (!block[block[i].NeighbourBlock[1]].bDefine &&
						block[block[i].NeighbourBlock[0]].bDefine)
					{
						if (block[block[i].NeighbourBlock[0]].order > block[i].order)
							block[block[i].NeighbourBlock[1]].order =
							block[i].order - 1;
						else
							block[block[i].NeighbourBlock[1]].order =
							block[i].order + 1;
						block[block[i].NeighbourBlock[1]].bDefine = TRUE;
					}
				}
				if (block[i].NeighbourNum > 2) //邻居数超过2
				{
					//寻找边缘长度最长的邻居
					int	MaxLengthIndex;
					int	MaxLength = 0;
					for (int j = 0; j < block[i].NeighbourNum; j++)
						if (block[i].NeighbourLength[j] > MaxLength)
						{
							MaxLength = block[i].NeighbourLength[j];
							MaxLengthIndex = j;
						}

					if (block[block[i].NeighbourBlock[MaxLengthIndex]].bDefine)
						//边缘长度最长的邻居级次已确定
					{
						if (block[block[i].NeighbourBlock[MaxLengthIndex]].order > block[i].order)
						{
							for (int j = 0; j < block[i].NeighbourNum; j++)
								if (j != MaxLengthIndex)
								{
									block[block[i].NeighbourBlock[j]].order =
										block[i].order - 1;
									block[block[i].NeighbourBlock[j]].bDefine = TRUE;
								}
						}
						else {
							for (int j = 0; j < block[i].NeighbourNum; j++)
								if (j != MaxLengthIndex)
								{
									block[block[i].NeighbourBlock[j]].order =
										block[i].order + 1;
									block[block[i].NeighbourBlock[j]].bDefine = TRUE;
								}
						}
					}
					else //边缘长度最长的邻居级次未定
					{
						//其他邻居的级次是否确定？
						BOOL	bOtherDefined = FALSE;
						int		DefinedIndex;
						for (int j = 0; j < block[i].NeighbourNum; j++)
							if (j != MaxLengthIndex &&
								block[block[i].NeighbourBlock[j]].bDefine)
							{
								DefinedIndex = j;
								bOtherDefined = TRUE;
								break;
							}

						if (bOtherDefined) //是
						{
							if (block[block[i].NeighbourBlock[DefinedIndex]].order > block[i].order)
							{
								for (int j = 0; j < block[i].NeighbourNum; j++)
									if (j != MaxLengthIndex)
									{
										block[block[i].NeighbourBlock[j]].order =
											block[i].order + 1;
										block[block[i].NeighbourBlock[j]].bDefine = TRUE;
									}
								block[block[i].NeighbourBlock[MaxLengthIndex]].order =
									block[i].order - 1;
								block[block[i].NeighbourBlock[MaxLengthIndex]].bDefine = TRUE;
							}
							else
							{
								for (int j = 0; j < block[i].NeighbourNum; j++)
									if (j != MaxLengthIndex)
									{
										block[block[i].NeighbourBlock[j]].order =
											block[i].order - 1;
										block[block[i].NeighbourBlock[j]].bDefine = TRUE;
									}
								block[block[i].NeighbourBlock[MaxLengthIndex]].order =
									block[i].order + 1;
								block[block[i].NeighbourBlock[MaxLengthIndex]].bDefine = TRUE;
							}
						}
						else //否
						{
							for (int j = 0; j < block[i].NeighbourNum; j++)
								if (j != MaxLengthIndex)
								{
									block[block[i].NeighbourBlock[j]].order =
										block[i].order + 1;
									block[block[i].NeighbourBlock[j]].bDefine = TRUE;
								}
							block[block[i].NeighbourBlock[MaxLengthIndex]].order =
								block[i].order - 1;
							block[block[i].NeighbourBlock[MaxLengthIndex]].bDefine = TRUE;
						}
					}
				}
			}
		cycle++;
	}
	if (cycle == cyclenum) return FALSE;//500
	//
	/*{
	for(i = 1; i <= blockNum; i++)
	if(!block[i].bDeleted && block[i].bDefine)
	{

	char s[256];
	sprintf(s, "num = %d, bw = %d, order = %d, Pts = %d, neighbour num = %d, %d,%d,%d,%d,%d",
	i,
	block[i].bw,
	block[i].order,
	block[i].TotalPts,
	block[i].NeighbourNum,
	block[i].NeighbourBlock[0],
	block[i].NeighbourBlock[1],
	block[i].NeighbourBlock[2],
	block[i].NeighbourBlock[3],
	block[i].NeighbourBlock[4]);
	AfxMessageBox(s);
	}
	}*/
	//
	return TRUE;

}
//判断级次确定是否结束
bool	CinterferomeryDlg::OrderFinished(BLOCKINF block[], int blockNum)
{
	BOOL	bFinished = TRUE;
	for (int i = 1; i <= blockNum; i++)
		if (!block[i].bDefine && !block[i].bDeleted) 
			bFinished = FALSE;
	return bFinished;
}

//void CinterferomeryDlg::OnRButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值  
//	if ((point.x<185&&point.x>10&&point.y<65&&point.y>10)|| (point.x<500 && point.x>325 && point.y<65 && point.y>10))
//	{
//		CMenu menu;
//		menu.LoadMenu(IDR_MENU3);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
//		CMenu* pPopup = menu.GetSubMenu(0);
//		ASSERT(pPopup != NULL);
//		ClientToScreen(&point);
//		/*CWnd* pWndPopupOwner = this;
//		while (pWndPopupOwner->GetStyle() & WS_CHILD)  pWndPopupOwner = pWndPopupOwner->GetParent();*/
//		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
//	}
//	else
//	{
//		CMenu menu;
//		menu.LoadMenu(IDR_MENU2);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
//		CMenu* pPopup = menu.GetSubMenu(0);
//		ASSERT(pPopup != NULL);
//		ClientToScreen(&point);
//		/*CWnd* pWndPopupOwner = this;
//		while (pWndPopupOwner->GetStyle() & WS_CHILD)  pWndPopupOwner = pWndPopupOwner->GetParent();*/
//		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
//	}
//	
//
//	CDialogEx::OnRButtonDown(nFlags, point);
//}

void CinterferomeryDlg::OnStart()
{
	// TODO: 在此添加命令处理程序代码
	//cvNamedWindow("我的摄像头", CV_WINDOW_AUTOSIZE);
	//VideoCapture captue(0);

	//captue.set(CV_CAP_PROP_FPS, 20);
	stop = !stop;
	//waitKey(1000);
	//Mat frame;
	//while (stop)
	//{
	//	if (!captue.read(frame))
	//	{
	//		waitKey(20);
	//		continue;
	//	}
	//	//captue >> frame;
	//	imshow("我的摄像头", frame);
	//	waitKey(20);
	//}
	//destroyWindow("我的摄像头");
	//captue.release();
	
}

//符号图的确定
void	CinterferomeryDlg::DefineSignMap(unsigned char(*image)[LENGTH],BLOCKINF	block[],int	blockNum,unsigned char(*sign)[LENGTH])
{
	//初始化符号图
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			sign[i][j] = UNDEFINED; // 0-无效 1-正 2-负

									//寻找边缘
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
		{
			if (j > 1 && ucharPtMap[i][j-1] && ucharPtMap[i][j] &&
				image[i][j - 1] != image[i][j])
				sign[i][j - 1] = sign[i][j] =
				GetSign(block, image[i][j - 1], image[i][j]);
			if (i > 1 && ucharPtMap[i - 1][j] && ucharPtMap[i][j] &&
				image[i - 1][j] != image[i][j])
				sign[i - 1][j] = sign[i][j] =
				GetSign(block, image[i - 1][j], image[i][j]);
			if (i > 1 && j > 1 && ucharPtMap[i - 1][j-1] && ucharPtMap[i][j] &&
				image[i - 1][j - 1] != image[i][j])
				sign[i - 1][j - 1] = sign[i][j] =
				GetSign(block, image[i - 1][j - 1], image[i][j]);
			if (i < TaskDispose::length - 1 && j > 1 && ucharPtMap[i+1][j - 1] && ucharPtMap[i][j] &&
				image[i + 1][j - 1] != image[i][j])
				sign[i + 1][j - 1] = sign[i][j] =
				GetSign(block, image[i + 1][j - 1], image[i][j]);
		}

	//产生符号图备份
	unsigned char(*sign1)[LENGTH];
	sign1 = new unsigned char[LENGTH][LENGTH];
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			sign1[i][j] = sign[i][j];


	//确定干涉图中每块的极值点位置
	DefineBlockExtremPoint(image, block, blockNum);

	//从边缘出发作膨胀处理
	BOOL	bCont = TRUE;
	while (bCont)
	{
		bCont = FALSE;
		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				if (sign[i][j] != UNDEFINED)
				{
					if (i > 1 && ucharPtMap[i-1][j] && sign[i - 1][j] == UNDEFINED)
					{
						sign1[i - 1][j] = sign[i][j];
						bCont = TRUE;
					}
					if (i < TaskDispose::length - 1 && ucharPtMap[i+1][j] && sign[i + 1][j] == UNDEFINED)
					{
						sign1[i + 1][j] = sign[i][j];
						bCont = TRUE;
					}
					if (j > 1 && ucharPtMap[i][j-1] && sign[i][j - 1] == UNDEFINED)
					{
						sign1[i][j - 1] = sign[i][j];
						bCont = TRUE;
					}
					if (j < TaskDispose::length - 1 && ucharPtMap[i][j+1] && sign[i][j + 1] == UNDEFINED)
					{
						sign1[i][j + 1] = sign[i][j];
						bCont = TRUE;
					}
					if (i > 1 && j > 1 && ucharPtMap[i-1][j-1] && sign[i - 1][j - 1] == UNDEFINED)
					{
						sign1[i - 1][j - 1] = sign[i][j];
						bCont = TRUE;
					}
					if (i < TaskDispose::length - 1 && j < TaskDispose::length - 1 && ucharPtMap[i+1][j+1] && sign[i + 1][j + 1] == UNDEFINED)
					{
						sign1[i + 1][j + 1] = sign[i][j];
						bCont = TRUE;
					}
					if (i > 1 && j < TaskDispose::length - 1 && ucharPtMap[i-1][j+1] && sign[i - 1][j + 1] == UNDEFINED)
					{
						sign1[i - 1][j + 1] = sign[i][j];
						bCont = TRUE;
					}
					if (i < TaskDispose::length - 1 && j > 1 && ucharPtMap[i+1][j-1] && sign[i + 1][j - 1] == UNDEFINED)
					{
						sign1[i + 1][j - 1] = sign[i][j];
						bCont = TRUE;
					}
				}

		//判断邻居数为1的块的膨胀是否已到达极值点，若到则作反符号填充处理
		for (int i = 1; i <= blockNum; i++)
			if (!block[i].bDeleted && block[i].NeighbourNum == 1 &&
				sign1[block[i].Px][block[i].Py] != UNDEFINED)
			{
				unsigned char	invertsign;
				if (sign1[block[i].Px][block[i].Py] == POSITIVE)
					invertsign = NEGATIVE;
				else
					invertsign = POSITIVE;
				for (int j = 0; j < TaskDispose::length; j++)
					for (int k = 0; k < TaskDispose::length; k++)
						if (image[j][k] == i && sign1[j][k] == UNDEFINED)
							sign1[j][k] = invertsign;
			}

		//复制备份
		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				sign[i][j] = sign1[i][j];
	}
	delete[] sign1;
}
//确定干涉图中每块的极值点位置
void	CinterferomeryDlg::DefineBlockExtremPoint(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum)
{
	for (int k = 1; k <= blockNum; k++)
		if (!block[k].bDeleted)
		{
			int	ExtremV;
			if (block[k].bw == BLACK) ExtremV = 255;
			else ExtremV = 0;
			for (int i = 0; i < TaskDispose::length; i++)
				for (int j = 0; j < TaskDispose::length; j++)
					if (image[i][j] == k)
					{
						if (block[k].bw == BLACK)
						{
							if (Image[i][j] < ExtremV)
							{
								ExtremV = Image[i][j];
								block[k].Px = i;
								block[k].Py = j;
							}
						}
						else
						{
							if (Image[i][j] > ExtremV)
							{
								ExtremV = Image[i][j];
								block[k].Px = i;
								block[k].Py = j;
							}
						}
					}
		}
}
//获取符号
unsigned char	CinterferomeryDlg::GetSign(BLOCKINF block[], int num1, int num2)
{
	int	MaxOrderNum;

	if (block[num1].order > block[num2].order) MaxOrderNum = num1;
	else MaxOrderNum = num2;

	if (block[MaxOrderNum].bw == BLACK) return POSITIVE;
	else return NEGATIVE;
}
//确定干涉图中明暗细条纹的位置
void CinterferomeryDlg::DefineThinFringe(unsigned char(*image)[LENGTH],BLOCKINF	block[],int	blockNum,unsigned char(*sign)[LENGTH],unsigned char(*fringe)[LENGTH])
{
	//初始化明暗条纹图
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			fringe[i][j] = UNDEFINED;

	//确定明暗细条纹位置
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (sign[i][j] != UNDEFINED &&
				((i > 1 && ucharPtMap[i - 1][j] && sign[i - 1][j] != sign[i][j]) ||
					(j > 1 && ucharPtMap[i][j-1] && sign[i][j - 1] != sign[i][j]) ||
					(i > 1 && j > 1 && ucharPtMap[i - 1][j-1] && sign[i - 1][j - 1] != sign[i][j]) ||
					(i < TaskDispose::length - 1 && j > 1 && ucharPtMap[i + 1][j-1] && sign[i + 1][j - 1] != sign[i][j])))
			{
				if (block[image[i][j]].bw == WHITE)
					fringe[i][j] = BRIGHT;
				else
					fringe[i][j] = DARK;
			}
}
//计算位相
void CinterferomeryDlg::CalculatePhase(unsigned char(*sign)[LENGTH], unsigned char(*fringe)[LENGTH])
{
	float xishu;
	/*if (TaskDispose::aperModel==1)
	{*/
		xishu = 128.0;
	//}
	//else
	//{
	//	xishu = 512.0;
	//}
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (ucharPtMap[i][j] && sign[i][j] != UNDEFINED)
			{
				int	DbrightMin = TaskDispose::length; //到亮条纹的最短距离
				int	Xbright, Ybright; // 到亮条纹最短距离的点
				int	DdarkMin = TaskDispose::length; //到暗条纹的最短距离
				int	Xdark, Ydark; // 到暗条纹最短距离的点

								  //扫描横线
				for (int k = 0; k < TaskDispose::length; k++)
				{
					if (fringe[i][k] == BRIGHT && abs(k - j) < DbrightMin)
					{
						DbrightMin = abs(k - j);
						Xbright = i;
						Ybright = k;
					}
					if (fringe[i][k] == DARK && abs(k - j) < DdarkMin)
					{
						DdarkMin = abs(k - j);
						Xdark = i;
						Ydark = k;
					}
				}

				//扫描纵线
				for (int k = 0; k < TaskDispose::length; k++)
				{
					if (fringe[k][j] == BRIGHT && abs(k - i) < DbrightMin)
					{
						DbrightMin = abs(k - i);
						Xbright = k;
						Ybright = j;
					}
					if (fringe[k][j] == DARK && abs(k - i) < DdarkMin)
					{
						DdarkMin = abs(k - i);
						Xdark = k;
						Ydark = j;
					}
				}

				//处理未找到亮暗条纹点的意外情况
				if (DbrightMin == TaskDispose::length)
				{
					double	d = 2.0*(double)TaskDispose::length;
					for (int ii = 0; ii < TaskDispose::length; ii++)
						for (int jj = 0; jj < TaskDispose::length; jj++)
							if (fringe[ii][jj] == BRIGHT)
							{
								double dd = sqrt((double)(i - ii)*(double)(i - ii) +
									(double)(j - jj)*(double)(j - jj));
								if (dd < d)
								{
									d = dd;
									Xbright = ii;
									Ybright = jj;
									DbrightMin = (int)d;
								}
							}
				}
				if (DdarkMin == TaskDispose::length)
				{
					double	d = 2.0*(double)TaskDispose::length;
					for (int ii = 0; ii < TaskDispose::length; ii++)
						for (int jj = 0; jj < TaskDispose::length; jj++)
							if (fringe[ii][jj] == DARK)
							{
								double dd = sqrt((double)(i - ii)*(double)(i - ii) +
									(double)(j - jj)*(double)(j - jj));
								if (dd < d)
								{
									d = dd;
									Xdark = ii;
									Ydark = jj;
									DdarkMin = (int)d;
								}
							}
				}

				//缺少亮条纹或暗条纹情况
				if (DbrightMin == TaskDispose::length || DdarkMin == TaskDispose::length)
				{
					WavePhase[i][j] = NON;
					continue;
				}

				int	max = Image[Xbright][Ybright];
				int	min = Image[Xdark][Ydark];
				int	v = Image[i][j];

				if (max < min)//一般情况下这不会发生
				{
					int	t;
					t = max;
					max = min;
					min = t;
				}

				if (v > max) v = max;
				if (v < min) v = min;
				WavePhase[i][j] = (int)(acos((double)(Image[i][j] - min) / (double)(max - min)) / PI*xishu);
				if (sign[i][j] == NEGATIVE) WavePhase[i][j] = -WavePhase[i][j];
			}
			else WavePhase[i][j] = NON;
}
// Continuning
bool CinterferomeryDlg::Cont()
{
	// initial flag matrix
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] == NON)
				m_flag[i][j] = 0;
			else m_flag[i][j] = UNPROCESSED;

			// unwrapping			                                                 
			unsigned char flagNum = 1;
			int	startX = 128;
			int	startY = 128;

			while (ContinueProcess(startX, startY))
			{
				m_flag[startX][startY] = flagNum;

				while (ContRight(flagNum) ||ContDown(flagNum) ||ContLeft(flagNum) ||ContUp(flagNum));
				//while (ContRight(flagNum));
				if (flagNum >= 50) return FALSE;

				flagNum++;
			}

			m_number = flagNum - 1;

			// Reset 3D view status
			//m_ThreeDInvert = FALSE;
			//m_ThreeDAngle = 0.0;

			// Get date and time
			//m_time = CTime::GetCurrentTime();

			return TRUE;
}
// Test if unwrapping should be continued
bool CinterferomeryDlg::ContinueProcess(int& Px, int& Py)
{
	for (int i = TaskDispose::length / 2; i < TaskDispose::length; i++)
		for (int j = TaskDispose::length / 2; j < TaskDispose::length; j++)
			if (m_flag[i][j] == UNPROCESSED)
			{
				Px = i;
				Py = j;
				return TRUE;
			}

	for (int i = TaskDispose::length / 2; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
			if (m_flag[i][j] == UNPROCESSED)
			{
				Px = i;
				Py = j;
				return TRUE;
			}

	for (int i = 0; i < TaskDispose::length / 2; i++)
		for (int j = TaskDispose::length / 2; j < TaskDispose::length; j++)
			if (m_flag[i][j] == UNPROCESSED)
			{
				Px = i;
				Py = j;
				return TRUE;
			}

	for (int i = 0; i < TaskDispose::length / 2; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
			if (m_flag[i][j] == UNPROCESSED)
			{
				Px = i;
				Py = j;
				return TRUE;
			}
	return FALSE;
}
// continue in right direction
bool CinterferomeryDlg::ContRight(unsigned char flagNum)
{
	BOOL	flag = FALSE;
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length - 1; j++)
			if (m_flag[i][j] == flagNum &&
				m_flag[i][j + 1] == UNPROCESSED)
			{
				Unwrap(WavePhase[i][j], WavePhase[i][j + 1]);
				m_flag[i][j + 1] = flagNum;
				flag = TRUE;
			}
	return flag;
}

// continue in down direction
bool CinterferomeryDlg::ContDown(unsigned char flagNum)
{
	BOOL	flag = FALSE;
	for (int i = 0; i < TaskDispose::length - 1; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (m_flag[i][j] == flagNum &&
				m_flag[i + 1][j] == UNPROCESSED)
			{
				Unwrap(WavePhase[i][j], WavePhase[i + 1][j]);
				m_flag[i + 1][j] = flagNum;
				flag = TRUE;
			}
	return flag;
}
// Unwrapping
void	CinterferomeryDlg::Unwrap(const int v1, int& v2)
{
	//while (v2 - v1 > 257)
	//	v2 -= 512;
	//while (v2 - v1 < -257)
	//	v2 += 512;
	//if (TaskDispose::aperModel==1)
	{
		int N = (v2 - v1) / 128;
		v2 -= N * 128;
		int temp = v2 - v1;
		if (temp > -66 && temp < 66)
		{
			return;
		}
		else if (temp > 65)
		{
			v2 -= 128;
		}
		else if (temp < -33)
		{
			v2 += 128;
		}
	}
	/*else
	{
		int N = (v2 - v1) / 512;
		v2 -= N * 512;
		int temp = v2 - v1;
		if (temp > -258 && temp < 258)
		{
			return;
		}
		else if (temp > 257)
		{
			v2 -= 512;
		}
		else if (temp < -257)
		{
			v2 += 512;
		}
	}*/
}
// continue in left direction
bool CinterferomeryDlg::ContLeft(unsigned char flagNum)
{
	BOOL	flag = FALSE;
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = TaskDispose::length - 1; j > 1; j--)
			if (m_flag[i][j] == flagNum &&
				m_flag[i][j - 1] == UNPROCESSED)
			{
				Unwrap(WavePhase[i][j], WavePhase[i][j - 1]);
				m_flag[i][j - 1] = flagNum;
				flag = TRUE;
			}
	return flag;
}

// continue in Up direction
bool CinterferomeryDlg::ContUp(unsigned char flagNum)
{
	BOOL	flag = FALSE;
	for (int i = TaskDispose::length - 1; i > 1; i--)
		for (int j = 0; j < TaskDispose::length; j++)
			if (m_flag[i][j] == flagNum &&
				m_flag[i - 1][j] == UNPROCESSED)
			{
				Unwrap(WavePhase[i][j], WavePhase[i - 1][j]);
				m_flag[i - 1][j] = flagNum;
				flag = TRUE;
			}
	return flag;
}
// Connection
void CinterferomeryDlg::Connect()
{
	float	*a, *b, *kk;
	int	i, j;
	int k, n, m, xb, yb, zb;
	float weight;

	weight = 15.0;
	n = (m_number + 3)*(m_number + 4) / 2;
	m = m_number + 3;

	if (m_number <= 1)  return;

	/*allocate memory*/

	a = (float *)new float[n];
	b = (float *)new float[m];
	kk = (float *)new float[m_number];
	/*initial matrix*/
	xb = m_number*(m_number + 1) / 2;
	yb = (m_number + 1)*(m_number + 2) / 2;
	zb = (m_number + 2)*(m_number + 3) / 2;
	for (k = 0; k < n; k++) a[k] = 0.0;
	for (k = 0; k < m; k++) b[k] = 0.0;

	/*creat flag matrix for seperated wavefront*/
	/* one : i, j - 1
	another: if flag == number+1
	w(i,j), j-1;
	if flag == number+2
	i, w(i,j);         */
	int judgenum;
	//if (TaskDispose::aperModel==1)
	//{
		judgenum = 16;
	//}
	//else
	//{
	//	judgenum = 128;
	//}
	for (i = 0; i < TaskDispose::length; i++) {
		int	i0, j0, i1, j1, i2, j2, i3, j3, f1, f2, f3, d1, d2, d3;

		j = 0;
	l:
		while ((j < TaskDispose::length) && ((*(m_flag + i*TaskDispose::length + j)) == 0)) j++;
		while ((j < TaskDispose::length) && ((*(m_flag + i*TaskDispose::length + j)) != 0)) j++;
		if (j == TaskDispose::length) continue;
		i0 = i;
		j0 = j;
		while ((j < TaskDispose::length) && ((*(m_flag + i*TaskDispose::length + j)) == 0)) j++;
		if (j == TaskDispose::length) f1 = -1;
		else {
			if ((*(m_flag + i*TaskDispose::length + j)) == (*(m_flag + i0*TaskDispose::length + j0 - 1))) f1 = -1;
			else {
				i1 = i;
				j1 = j;
				f1 = 1;
			}
		}
		i = i0;
		j = j0;
		if (i == 0) f2 = -1;
		else {
			i--;
			j--;
			while ((i > 0) && (m_flag[i][j] == 0) ||
				m_flag[i][j] > m_number) i--;
			if (i == 0) f2 = -1;
			else {
				if ((*(m_flag + i*TaskDispose::length + j)) == (*(m_flag + i0*TaskDispose::length + j0 - 1))) f2 = -1;
				else {
					i2 = i;
					j2 = j;
					f2 = 1;
				}
			}
		}
		i = i0;
		j = j0;
		if (i == TaskDispose::length - 1) f3 = -1;
		else {
			i++;
			j--;
			while ((i < TaskDispose::length) && ((*(m_flag + i*TaskDispose::length + j)) == 0)) i++;
			if (i == TaskDispose::length) f3 = -1;
			else {
				if ((*(m_flag + i*TaskDispose::length + j)) == (*(m_flag + i0*TaskDispose::length + j0 - 1))) f3 = -1;
				else {
					i3 = i;
					j3 = j;
					f3 = 1;
				}
			}
		}
		if ((f1 == -1) && (f2 == -1) && (f3 == -1)) {
			j = j0 + 1;
			i = i0;
			goto l;
		}
		if (f1 == -1) d1 = judgenum;
		else d1 = j1 - j0 + 1;
		if (f2 == -1) d2 = judgenum;
		else d2 = i0 - i2;
		if (f3 == -1) d3 = judgenum;
		else d3 = i3 - i0;
		if (d1 > d2) {
			if (d2 > d3) {
				m_flag[i0][j0] = m_number + 1;
				WavePhase[i0][j0] = i3;
			}
			else {
				m_flag[i0][j0] = m_number + 1;
				WavePhase[i0][j0] = i2;
			}
		}
		else {
			if (d1 > d3) {
				m_flag[i0][j0] = m_number + 1;
				WavePhase[i0][j0] = i3;
			}
			else {
				m_flag[i0][j0] = m_number + 2;
				WavePhase[i0][j0] = j1;
			}
		}
		j = j0 + 1;
		i = i0;
		goto l;
	}

	/*generate matrix on Pi */
	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++)
			if (m_flag[i][j] != 0) {
				k = m_flag[i][j];
				if (k <= m_number)
					a[k*(k + 1) / 2 - 1] += (float)1.0;
				else {
					int k1, k2, t;
					k1 = m_flag[i][j - 1];
					t = WavePhase[i][j];
					if (k == m_number + 1) k2 = m_flag[t][j - 1];
					else k2 = m_flag[i][t];
					if (k1 > k2) {
						t = k1;
						k1 = k2;
						k2 = t;
					}
					a[k1*(k1 + 1) / 2 - 1] += (float)(1.0*weight);
					a[k2*(k2 + 1) / 2 - 1] += (float)(1.0*weight);
					a[k2*(k2 - 1) / 2 + k1 - 1] -= (float)(1.0*weight);
				}
			}
	for (i = 0; i< m_number; i++) {
		kk[i] = 1.0;
		a[(i + 1)*(i + 2) / 2 - 1] *= kk[i];
	}

	/*generate matrix*/
	for (i = 0; i<TaskDispose::length; i++)
		for (j = 0; j<TaskDispose::length; j++)
			if (m_flag[i][j] != 0) {
				if (m_flag[i][j] <= m_number) {
					float ii, jj;
					float t;
					ii = (float)(i - TaskDispose::length / 2);
					jj = (float)(j - TaskDispose::length / 2);
					k = m_flag[i][j];
					a[xb + k - 1] += ii*kk[k - 1];
					a[xb + m_number] += ii*ii*kk[k - 1];
					a[yb + k - 1] += jj*kk[k - 1];
					a[yb + m_number] += ii*jj*kk[k - 1];
					a[yb + m_number + 1] += jj*jj*kk[k - 1];
					t = ii*ii + jj*jj;
					a[zb + k - 1] += t*kk[k - 1];
					a[zb + m_number] += t*ii*kk[k - 1];
					a[zb + m_number + 1] += t*jj*kk[k - 1];
					a[zb + m_number + 2] += t*t*kk[k - 1];
					b[k - 1] += (float)WavePhase[i][j] * kk[k - 1];
					b[m_number] += (float)(WavePhase[i][j])
						*ii*kk[k - 1];
					b[m_number + 1] += (float)(WavePhase[i][j])
						*jj*kk[k - 1];
					b[m_number + 2] += (float)(WavePhase[i][j])
						*t*kk[k - 1];
				}
				else {
					int	k1, k2, w1, w2, t1;
					float i1, i2, j1, j2;
					float t;
					i1 = (float)(i - TaskDispose::length / 2);
					j1 = (float)(j - 1 - TaskDispose::length / 2);
					k1 = m_flag[i][j - 1];
					w1 = WavePhase[i][j - 1];
					t1 = WavePhase[i][j];
					if (m_flag[i][j] == m_number + 1) {
						i2 = (float)(t1 - TaskDispose::length / 2);
						j2 = (float)(j - 1 - TaskDispose::length / 2);
						k2 = m_flag[t1][j - 1];
						w2 = WavePhase[t1][j - 1];
					}
					else {
						i2 = (float)(i - TaskDispose::length / 2);
						j2 = (float)(t1 - TaskDispose::length / 2);
						k2 = m_flag[i][t1];
						w2 = WavePhase[i][t1];
					}
					a[xb + k1 - 1] += (i1 - i2)*weight;
					a[xb + k2 - 1] += (i2 - i1)*weight;
					a[xb + m_number] += (i1 - i2)*(i1 - i2)*weight;
					a[yb + k1 - 1] += (j1 - j2)*weight;
					a[yb + k2 - 1] += (j2 - j1)*weight;
					a[yb + m_number] += (i1 - i2)*(j1 - j2)*weight;
					a[yb + m_number + 1] += (j1 - j2)*(j1 - j2)*weight;
					t = (i1 + i2)*(i1 - i2) + (j1 + j2)*(j1 - j2);
					a[zb + k1 - 1] += t*weight;
					a[zb + k2 - 1] += -t*weight;
					a[zb + m_number] += t*(i1 - i2)*weight;
					a[zb + m_number + 1] += t*(j1 - j2)*weight;
					a[zb + m_number + 2] += t*t*weight;
					b[k1 - 1] += (float)(w1 - w2)*weight;
					b[k2 - 1] += (float)(w2 - w1)*weight;
					b[m_number] += (float)(w1 - w2)
						*(i1 - i2)*weight;
					b[m_number + 1] += (float)(w1 - w2)
						*(j1 - j2)*weight;
					b[m_number + 2] += (float)(w1 - w2)
						*t*weight;
				}
			}


	/*solving the equation*/
	Ldlt(a, b, m_number + 3);

	for (i = 0; i<m_number - 1; i++) {
		float	t;
		t = b[i];
		while (t - b[i + 1]< -257) {
			t += 512;
		}
		while (t - b[i + 1]>  257) {
			t -= 512;
		}
		b[i + 1] = t;
	}

	/*removing the pision */
	for (i = 0; i<TaskDispose::length; i++)
		for (j = 0; j<TaskDispose::length; j++)
			if (m_flag[i][j] != 0) {
				k = m_flag[i][j];
				if (k <= m_number)
					WavePhase[i][j] -= (int)(b[k - 1]);
				else WavePhase[i][j] = NON;
			}

	delete[] a;
	delete[] b;
	delete[] kk;

}
// Soving equation by ldlt		
void	CinterferomeryDlg::Ldlt(float a[], float b[], int n)
{
	int	i, j, k;
	float	t, t0, d;
	if (fabs(a[0]) < 1.0e-30) return;
	a[0] = (float)(1.0 / a[0]);
	for (i = 1; i < n; i++) {
		for (j = 0; j <= i - 1; j++)
			if (j != 0) {
				t = 0;
				for (k = 0; k <= j - 1; k++)
					t += a[i*(i + 1) / 2 + k] * a[j*(j + 1) / 2 + k];
				a[i*(i + 1) / 2 + j] -= t;
			}
		d = a[i*(i + 1) / 2 + i];
		for (j = 0; j <= i - 1; j++) {
			t0 = a[i*(i + 1) / 2 + j];
			a[i*(i + 1) / 2 + j] = t0*a[j*(j + 1) / 2 + j];
			d -= t0*a[i*(i + 1) / 2 + j];
		}
		if (fabs(d) < 1.0e-30) return;
		a[i*(i + 1) / 2 + i] = (float)(1.0 / d);
	}
	for (i = 1; i < n; i++) {
		t = 0.0;
		for (k = 0; k <= i - 1; k++)
			t += a[i*(i + 1) / 2 + k] * b[k];
		b[i] -= t;
	}
	b[n - 1] *= a[n*(n - 1) / 2 + n - 1];
	for (i = n - 2; i >= 0; i--) {
		t = b[i] * a[i*(i + 1) / 2 + i];
		for (k = i + 1; k < n; k++) t -= a[k*(k + 1) / 2 + i] * b[k];
		b[i] = t;
	}
}
// Moving aligment error
void	CinterferomeryDlg::Sms()
{
	//Get Fringe Number
	m_fringeNumber = GetPV()*(float)2.0;

	switch (m_Type)
	{
	case PLANE:	sms_p();
		break;
	case SPHERE: sms_s();
		break;
	case ORIGINAL:
		break;
	}

	// adjust 3D View display coefficient automatically
	//if (GetPV() > 1.2) {
		//char	ss[80];
		//sprintf_s(ss, "%5.2f", (float)(0.07*5.0) / GetPV());
		//m_ThreeDViewK = (float)atof(ss);
	//}
}
float	CinterferomeryDlg::GetPV()
{
	float xishu;
	//if (TaskDispose::aperModel==1)
	//{
		xishu = 256.0;
	/*}
	else
	{
		xishu = 1024.0;
	}*/
	int	i, j, l, pt, max[NUMBER], min[NUMBER];
	long int	sum;

	if (GetPTS() == 0) return 0.0;
	/*get P & V value*/
	for (i = 0; i < NUMBER; i++) {
		max[i] = NON;
		min[i] = -NON - 1;
	}

	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++) {
			int	t;
			t = WavePhase[i][j];
			if (t != NON) {
				for (l = NUMBER - 1; l >= 0; l--)
					if (t <= max[l]) break;
				l++;
				if (l < NUMBER) {
					pt = l;
					for (l = NUMBER - 1; l > pt; l--) max[l] = max[l - 1];
					max[pt] = t;
				}
			}
		}

	sum = 0;
	for (i = 0; i < NUMBER; i++) sum += max[i];
	max[0] = (int)(sum / NUMBER);

	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++) {
			int	t;
			t = WavePhase[i][j];
			if (t != NON) {
				for (l = NUMBER - 1; l >= 0; l--)
					if (t >= min[l]) break;
				l++;
				if (l < NUMBER) {
					pt = l;
					for (l = NUMBER - 1; l > pt; l--) min[l] = min[l - 1];
					min[pt] = t;
				}
			}
		}

	sum = 0;
	for (i = 0; i < NUMBER; i++) sum += min[i];
	min[0] = (int)(sum / NUMBER);

	return((float)(max[0] - min[0])) / xishu;
}
// Get Effective points
int		CinterferomeryDlg::GetPTS()
{
	int	num = 0;

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (!(NON== WavePhase[i][j]))
				num++;
	return num;
}
// moving tilt, piston
void	CinterferomeryDlg::sms_p()
{
	float	*a, *b;
	int	i, j;
	// Exception processing                          
	if (GetPTS() == 0) return;
	a = (float *)new float[6];
	b = (float *)new float[3];
	for (i = 0; i < 6; i++) a[i] = 0;
	for (i = 0; i < 3; i++) b[i] = 0;
	/*generate matrix*/
	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON) {
				a[0] += (float)1;
				a[1] += (float)i;
				a[2] += (float)i*(float)i;
				a[3] += (float)j;
				a[4] += (float)i*(float)j;
				a[5] += (float)j*(float)j;
				b[0] += (float)WavePhase[i][j];
				b[1] += (float)WavePhase[i][j]
					* (float)i;
				b[2] += (float)WavePhase[i][j]
					* (float)j;
			}

	/*solving the equation*/
	Ldlt(a, b, 3);
	/*removing the pision & till */
	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON)
				WavePhase[i][j] -=
				(int)(b[0] + (float)i*b[1] + (float)j*b[2]);

	delete[] a;
	delete[] b;
}
// moving tilt, piston and focus
void	CinterferomeryDlg::sms_s()
{
	float	*a, *b;
	int	i, j;
	float	K = 1;
	// Exception processing
	if (GetPTS() == 0) return;
	a = (float *)new float[10];
	b = (float *)new float[4];
	for (i = 0; i < 10; i++) {
		a[i] = 0.0;
	}
	for (i = 0; i < 4; i++) {
		b[i] = 0.0;
	}
	/*generate matrix*/

	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON) {
				float ii, jj;
				float t;
				ii = (float)(i - TaskDispose::length / 2);
				jj = (float)(j - TaskDispose::length / 2);
				a[0] += 1;
				a[1] += ii;
				a[2] += ii*ii;
				a[3] += (float)jj*K;
				a[4] += (float)(ii*jj)*K;
				a[5] += (float)(jj*jj)*K*K;
				t = (float)(ii*ii)
					+ (float)(jj*jj)*K*K;
				a[6] += t;
				a[7] += t*ii;
				a[8] += t*(float)jj*K;
				a[9] += t*t;
				b[0] += WavePhase[i][j];
				b[1] += (float)WavePhase[i][j]
					* ii;
				b[2] += (float)WavePhase[i][j]
					* (float)jj*K;
				b[3] += (float)WavePhase[i][j]
					* t;
			}

	/*solving the equation*/

	Ldlt(a, b, 4);

	/*removing the pision & till & defocus*/

	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON)
				WavePhase[i][j] -=
				(int)((b[0] + (i - TaskDispose::length / 2)*b[1] + K*(j - TaskDispose::length / 2)*b[2] +
					((float)(i - TaskDispose::length / 2)
						*(float)(i - TaskDispose::length / 2) + (float)(j - TaskDispose::length / 2)
						*(float)(j - TaskDispose::length / 2)*K*K)*b[3]));

	delete[]a;
	delete[]b;
}
// Noise Removing
void	CinterferomeryDlg::NoiseRemove()
{
	//	if(m_measureBand == NOFILTER) return;
	
	double* aa = (double *)GlobalAllocPtr(GHND,
		(DWORD)TaskDispose::length*(DWORD)TaskDispose::length*(DWORD)sizeof(double));
	double* bb = (double *)GlobalAllocPtr(GHND,
		(DWORD)TaskDispose::length*(DWORD)TaskDispose::length*(DWORD)sizeof(double));
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			*(aa + i*TaskDispose::length + j) = *(bb + i*TaskDispose::length + j) = 0.0;


	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON)
				*(aa + i*TaskDispose::length + j) = (double)(WavePhase[i][j]);


	FFT2D(TaskDispose::length, aa, bb);

	for (int i = 0; i < TaskDispose::length / 2; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

		}

	for (int i = TaskDispose::length / 2; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;
		}

	//Remove noise in the frequency domain 

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
		{
			double x = (double)(i - TaskDispose::length / 2);
			double y = (double)(j - TaskDispose::length / 2);

			double cutf;
			if (m_Type == ORIGINAL) cutf = 256.0;
			else {
				switch (m_measureBand)
				{
				case BAND1:
					cutf = 4.0;
					break;
				case BAND2:
					cutf = 6.0;
					break;
				case BAND3:
					cutf = 20.0; //8.0
					break;
				case NOFILTER:
					cutf = 512.0;
					break;

				}
			}
			if (!m_bScanWay && m_Type != ORIGINAL) cutf = 150.0;//3
			//if (TaskDispose::aperModel==1)
			//{
				cutf = 25;
			/*}
			else
			{
				cutf = 150;
			}*/
			if (sqrt(x*x + y*y) > cutf)
			{
				(*(aa + i*TaskDispose::length + j)) = (*(bb + i*TaskDispose::length + j)) = 0.0;
			}
		}

	for (int i = 0; i < TaskDispose::length / 2; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;
 			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

		}

	for (int i = TaskDispose::length / 2; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;
		}



	IFFT2D(TaskDispose::length, aa, bb);


	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON)
				WavePhase[i][j] = (int)(*(aa + i*TaskDispose::length + j)*1.0);


	GlobalFreePtr(aa);
	GlobalFreePtr(bb);
}
void	CinterferomeryDlg::FFT2D(int m, double *aa, double *bb)
{
	int		i, j;
	double*	a = new double[m];
	double*	b = new double[m];



	for (i = 0; i < m; i++) {
		for (j = 0; j < m; j++) {
			*(a + j) = *(aa + i*m + j);	*(b + j) = *(bb + i*m + j);
		}

		FFT(m, a, b);

		for (j = 0; j < m; j++) {
			*(aa + i*m + j) = *(a + j);	*(bb + i*m + j) = *(b + j);
		}
	}

	for (j = 0; j < m; j++) {
		for (i = 0; i < m; i++) {
			*(a + i) = *(aa + i*m + j);	*(b + i) = *(bb + i*m + j);
		}

		FFT(m, a, b);

		for (i = 0; i < m; i++) {
			*(aa + i*m + j) = *(a + i);	*(bb + i*m + j) = *(b + i);
		}
	}
	delete[] a;
	delete[] b;
}
// subroutine for Reverse FFT2D
void	CinterferomeryDlg::IFFT2D(int m, double *aa, double *bb)
{
	int		i, j;
	double*	a = new double[m];
	double*	b = new double[m];

	for (j = 0; j < m; j++) {
		for (i = 0; i < m; i++) {
			*(a + i) = *(aa + i*m + j);	*(b + i) = *(bb + i*m + j);
		}

		IFFT(m, a, b);

		for (i = 0; i < m; i++) {
			*(aa + i*m + j) = *(a + i);	*(bb + i*m + j) = *(b + i);
		}
	}

	for (i = 0; i < m; i++) {
		for (j = 0; j < m; j++) {
			*(a + j) = *(aa + i*m + j);	*(b + j) = *(bb + i*m + j);
		}

		IFFT(m, a, b);

		for (j = 0; j < m; j++) {
			*(aa + i*m + j) = *(a + j);	*(bb + i*m + j) = *(b + j);
		}
	}


	delete[] a;
	delete[] b;
}
void	CinterferomeryDlg::FFT(int m, double *a, double *b)
{
	int	k, j = 0, le = 1, le1, le11;
	register	int	i, l;
	double	t1, t2, u1, u2, k1, w1, w2;
	int	n;

	// get n
	int tm = m;
	n = 0;
	while (tm != 0)
	{
		tm = tm / 2;
		n++;
	}
	n--;

	for (i = 0; i<m - 1; i++) {
		if (j>i) {
			t1 = *(a + j);		t2 = *(b + j);
			*(a + j) = *(a + i);	*(b + j) = *(b + i);
			*(a + i) = t1;		*(b + i) = t2;
		}
		k = m / 2;
		while (j >= k) {
			j -= k;	k = k / 2;
		}
		j += k;
	}
	for (l = 1; l <= n; l++) {
		le *= 2;	le1 = le / 2;
		u1 = 1;	u2 = 0;
		w1 = (cos(PI / le1));	w2 = (sin(PI / le1));
		le11 = le1 - 1;
		for (j = 0; j <= le11; j++) {
			for (i = j; i <= m - 1; i += le) {
				t1 = (*(a + i + le1))*u1 + (*(b + i + le1))*u2;
				t2 = (*(b + i + le1))*u1 - (*(a + i + le1))*u2;
				*(a + i + le1) = (*(a + i)) - t1;
				*(b + i + le1) = (*(b + i)) - t2;
				(*(a + i)) += t1;
				(*(b + i)) += t2;
			}
			k1 = u1*w1 - u2*w2;
			u2 = u1*w2 + u2*w1;
			u1 = k1;
		}
	}
	/*
	for(i=0;i<m;i++){
	*(a+i)=*(a+i)/m;
	*(b+i)=*(b+i)/m;
	}*/
}
// Inverse FFT subroutine 
// the n th power of 2 is m
void	CinterferomeryDlg::IFFT(int m, double *a, double *b)
{
	int	k, j = 0, le = 1, le1, le11;
	register	int	i, l;
	double	t1, t2, u1, u2, k1, w1, w2;
	int	n;

	// get n
	int tm = m;
	n = 0;
	while (tm != 0)
	{
		tm = tm >> 1;
		n++;
	}
	n--;

	for (i = 0; i<m - 1; i++) {
		if (j>i) {
			t1 = *(a + j);		t2 = *(b + j);
			*(a + j) = *(a + i);	*(b + j) = *(b + i);
			*(a + i) = t1;		*(b + i) = t2;
		}
		k = m / 2;
		while (j >= k) {
			j -= k;	k = k / 2;
		}
		j += k;
	}
	for (l = 1; l <= n; l++) {
		le *= 2;	le1 = le / 2;
		u1 = 1;	u2 = 0;
		w1 = (cos(PI / le1));	w2 = (sin(PI / le1));
		le11 = le1 - 1;
		for (j = 0; j <= le11; j++) {
			for (i = j; i <= m - 1; i += le) {
				t1 = (*(a + i + le1))*u1 + (*(b + i + le1))*u2;
				t2 = (*(b + i + le1))*u1 - (*(a + i + le1))*u2;
				*(a + i + le1) = (*(a + i)) - t1;
				*(b + i + le1) = (*(b + i)) - t2;
				(*(a + i)) += t1;
				(*(b + i)) += t2;
			}
			k1 = u1*w1 + u2*w2;
			u2 = -u1*w2 + u2*w1;
			u1 = k1;
		}
	}
	for (i = 0; i<m; i++) {
		*(a + i) = *(a + i) / (double)m;
		*(b + i) = *(b + i) / (double)m;
	}
}
void	CinterferomeryDlg::Smooth()
{
	int	*a, *b;
	int	i, j, j1, j2;
	a = (int *)new int[LENGTH];
	b = (int *)new int[LENGTH];
	int	a1 = TaskDispose::length - 1;
	int	b1 = -1;
	for (i = TaskDispose::length / 2 - 1; i >= 0; i--)
	{
		j = 0;
		while (j != TaskDispose::length) {
			while ((j < TaskDispose::length) && (WavePhase[i][j] == NON)) j++;
			if (j == TaskDispose::length)	continue;
			else
			{
				j1 = j;
				while ((j < TaskDispose::length) && (WavePhase[i][j] != NON))
				{
					j2 = j;
					b[j] = WavePhase[i][j];
					j++;
				}
				ssmo1(j1, j2, a, b);
				for (j = j1; j <= j2; j++) WavePhase[i][j] = a[j];
			}
		}
	}

	a1 = TaskDispose::length - 1;	b1 = -1;
	for (i = TaskDispose::length / 2; i <= TaskDispose::length - 1; i++)
	{
		j = 0;
		while (j != TaskDispose::length) {
			while ((j < TaskDispose::length) && (WavePhase[i][j] == NON)) j++;
			if (j == TaskDispose::length)	continue;
			else
			{
				j1 = j;
				while ((j < TaskDispose::length) && (WavePhase[i][j] != NON))
				{
					j2 = j;
					b[j] = WavePhase[i][j];
					j++;
				}
				ssmo1(j1, j2, a, b);
				for (j = j1; j <= j2; j++) WavePhase[i][j] = a[j];
			}
		}
	}

	/*at another direction*/
	a1 = TaskDispose::length - 1;	b1 = -1;
	for (j = TaskDispose::length / 2 - 1; j >= 0; j--)
	{
		i = 0;
		while (i != TaskDispose::length) {
			while ((i < TaskDispose::length) && (WavePhase[i][j] == NON)) i++;
			if (i == TaskDispose::length)	continue;
			else
			{
				j1 = i;
				while ((i < TaskDispose::length) && (WavePhase[i][j] != NON))
				{
					j2 = i;
					b[i] = WavePhase[i][j];
					i++;
				}
				ssmo1(j1, j2, a, b);
				for (i = j1; i <= j2; i++) WavePhase[i][j] = a[i];
			}
		}
	}
	a1 = TaskDispose::length - 1;	b1 = -1;
	for (j = TaskDispose::length / 2; j < TaskDispose::length; j++)
	{
		i = 0;
		while (i != TaskDispose::length) {
			while ((i < TaskDispose::length) && (WavePhase[i][j] == NON)) i++;
			if (i == TaskDispose::length)	continue;
			else
			{
				j1 = i;
				while ((i < TaskDispose::length) && (WavePhase[i][j] != NON))
				{
					j2 = i;
					b[i] = WavePhase[i][j];
					i++;
				}
				ssmo1(j1, j2, a, b);
				for (i = j1; i <= j2; i++) WavePhase[i][j] = a[i];
			}
		}
	}
	delete[] a;
	delete[] b;

}
void	CinterferomeryDlg::ssmo1(int j1, int j2, int *a, int *b)
{
	int	i, j;
	long int t[5];

	for (i = j1; i <= j2; i++) {
		for (j = 0; j < 5; j++) {
			if ((j + i - 2 < j1) || (j + i - 2 > j2)) 	t[j] = b[i];
			else	t[j] = b[j + i - 2];
		}
		a[i] = int(-(t[0] + t[4]) / 12
			+ (t[1] + t[3]) / 3
			+ t[2] / 2);
	}
}
float	CinterferomeryDlg::GetRms()// Get Rms
{
	int	num = 0;
	float	v = 0.0;
	float divnum;
	//if (TaskDispose::aperModel==1)
	//{
		divnum = 256.0;
	/*}
	else
	{
		divnum = 1024.0;
	}*/
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON)
			{
				num++;
				v += (float)WavePhase[i][j] / divnum;//1024.0
			}

	if (num == 0) return 0.0;

	float	ave = v / num;
	float	sum = 0.0;

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON)
			{
				float	w = (float)WavePhase[i][j] / divnum;//1024.0
				sum += (w - ave)*(w - ave);
			}

	return ((float)sqrt((double)(sum / num)));
}
float	CinterferomeryDlg::GetN()//求光圈数
{

	return(GetPV()*(float)2.0); //2006年12月1日修改

	float	Result;
	int(*WavePhaseTemp)[LENGTH]; //存放临时备份波面数据	 
	WavePhaseTemp = new int[LENGTH][LENGTH];

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			WavePhaseTemp[i][j] = WavePhase[i][j]; //留备份

												   //求光圈数
	sms_s();
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (WavePhase[i][j] != NON)
				WavePhase[i][j] -= WavePhaseTemp[i][j];
	Result = (float)(fabs(GetPV())*2.0);

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			WavePhase[i][j] = WavePhaseTemp[i][j]; //恢复备份

	delete[] WavePhaseTemp;

	return Result;
}
float	CinterferomeryDlg::GetDeltN(int flag)//求局部光圈数
{
	return(GetEms()*(float)2.0);// 2006年12月1日修改

	if (flag == SPHERE)//对球面
	{
		return(GetPV()*(float)2.0);
	}
	if (flag == PLANE)//对平面
	{
		int(*WavePhaseTemp)[LENGTH]; //存放临时备份波面数据	 
		WavePhaseTemp = new int[LENGTH][LENGTH];

		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				WavePhaseTemp[i][j] = WavePhase[i][j]; //留备份

													   //求取局部光圈数
		float  Result;
		sms_s();
		Smooth();
		Smooth();
		Result = GetPV()*(float)2.0;

		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				WavePhase[i][j] = WavePhaseTemp[i][j]; //恢复备份

		delete[] WavePhaseTemp;

		return Result;
	}

	return (float)0.0;//在给定输入参数之外时
}
float	CinterferomeryDlg::GetEms()
{
	float divnum;
	//if (TaskDispose::aperModel == 1)
	//{
		divnum = 256.0;
	/*}
	else
	{
		divnum = 1024.0;
	}*/
	if (fabs(GetMaxWavephase()) > fabs(GetMinWavephase()))
		return (float)fabs(GetMaxWavephase()) / divnum;//1024.0
	else
		return (float)fabs(GetMinWavephase()) / divnum;//1024.0

	//	return(GetPV()/(float)1.58);
}
int CinterferomeryDlg::GetMaxWavephase()
{
	int	i, j, l, pt, max[NUMBER];
	long int	sum;

	if (GetPTS() == 0) return 0;
	/*get P value*/
	for (i = 0; i < NUMBER; i++) {
		max[i] = NON;
	}

	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++) {
			int	t;
			t = WavePhase[i][j];
			if (t != NON) {
				for (l = NUMBER - 1; l >= 0; l--)
					if (t <= max[l]) break;
				l++;
				if (l < NUMBER) {
					pt = l;
					for (l = NUMBER - 1; l > pt; l--) max[l] = max[l - 1];
					max[pt] = t;
				}
			}
		}

	sum = 0;
	for (i = 0; i < NUMBER; i++) sum += max[i];
	max[0] = (int)(sum / NUMBER);

	return max[0];

}
int CinterferomeryDlg::GetMinWavephase()
{
	int	i, j, l, pt, min[NUMBER];
	long int	sum;

	if (GetPTS() == 0) return 0;
	for (i = 0; i < NUMBER; i++) {
		min[i] = -NON - 1;
	}

	for (i = 0; i < TaskDispose::length; i++)
		for (j = 0; j < TaskDispose::length; j++) {
			int	t;
			t = WavePhase[i][j];
			if (t != NON) {
				for (l = NUMBER - 1; l >= 0; l--)
					if (t >= min[l]) break;
				l++;
				if (l < NUMBER) {
					pt = l;
					for (l = NUMBER - 1; l > pt; l--) min[l] = min[l - 1];
					min[pt] = t;
				}
			}
		}

	sum = 0;
	for (i = 0; i < NUMBER; i++) sum += min[i];
	min[0] = (int)(sum / NUMBER);

	return min[0];

}
//干涉图滤波处理
void	CinterferomeryDlg::NoiseRemoveInterferogram(unsigned char(*image)[LENGTH])
{

	double* aa = (double *)GlobalAllocPtr(GHND,
		(DWORD)TaskDispose::length*(DWORD)TaskDispose::length*(DWORD)sizeof(double));
	double* bb = (double *)GlobalAllocPtr(GHND,
		(DWORD)TaskDispose::length*(DWORD)TaskDispose::length*(DWORD)sizeof(double));
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			*(aa + i*TaskDispose::length + j) = *(bb + i*TaskDispose::length + j) = 0.0;


	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (ucharPtMap[i][j])
				*(aa + i*TaskDispose::length + j) = (double)(image[i][j]);


	FFT2D(TaskDispose::length, aa, bb);

	for (int i = 0; i < TaskDispose::length / 2; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

		}

	for (int i = TaskDispose::length / 2; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;
		}

	//Remove noise in the frequency domain 

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
		{
			double x = (double)(i - TaskDispose::length / 2);
			double y = (double)(j - TaskDispose::length / 2);

			double cutf = 28;//50
			if (TaskDispose::aperModel==1)
			{
				cutf = 28;
			}
			if (sqrt(x*x + y*y) > cutf)
			{
				(*(aa + i*TaskDispose::length + j)) = (*(bb + i*TaskDispose::length + j)) = 0.0;
			}
		}

	for (int i = 0; i < TaskDispose::length / 2; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i + TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

		}

	for (int i = TaskDispose::length / 2; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length / 2; j++)
		{
			double temp;
			temp = *(aa + i*TaskDispose::length + j);
			*(aa + i*TaskDispose::length + j) =
				*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(aa + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;

			temp = *(bb + i*TaskDispose::length + j);
			*(bb + i*TaskDispose::length + j) =
				*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2);
			*(bb + (i - TaskDispose::length / 2)*TaskDispose::length + j + TaskDispose::length / 2) = temp;
		}


	IFFT2D(TaskDispose::length, aa, bb);

	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			if (ucharPtMap[i][j])
			{
				if (*(aa + i*TaskDispose::length + j) > 255.0)
					image[i][j] = 255;
				else
					if (*(aa + i*TaskDispose::length + j) < 0)
						image[i][j] = 0;
					else
						image[i][j] = (int)(*(aa + i*TaskDispose::length + j));
			}

	GlobalFreePtr(aa);
	GlobalFreePtr(bb);
}
//抓取轮廓中心  
void CinterferomeryDlg::GetContourCenter(std::vector< cv::Point>  contour, Point &p, int &radius)
{
	//重心法抓中心点  
	double avg_px = 0, avg_py = 0;
	int minx, miny, maxx, maxy;
	minx = contour[0].x;
	maxx = contour[0].x;
	miny = contour[0].y;
	maxy = contour[0].y;
	if (contour.size()<10)
	{
		p = contour.at((int)contour.size()*float(0.35));
	}
	else
	{
		for (int i = 0; i < contour.size();i++)
		{
				avg_px += contour.at(i).x;
				avg_py += contour.at(i).y;
				if (minx > contour.at(i).x)
					minx = contour.at(i).x;
				if (maxx < contour.at(i).x)
					maxx = contour.at(i).x;
				if (miny > contour.at(i).y)
					miny = contour.at(i).y;
				if (maxy < contour.at(i).y)
					maxy = contour.at(i).y;
		}
		p.x = avg_px / contour.size();
		p.y = avg_py / contour.size();
		radius = min(maxx-minx, maxy-miny)/2;
	}
}


void CinterferomeryDlg::OnAutomodel()
{
	// TODO: 在此添加命令处理程序代码
	//m_StatBar->SetText(_T("自动选择孔径模式"), 0, 0);
	TaskDispose::aperModel = 0;
	/*CRect clientrect;
	GetClientRect(clientrect);
	CRect  rect;
	GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(rect);
	int x = clientrect.Size().cx*0.15;
	int y = clientrect.Size().cy*0.15;
	GetDlgItem(IDC_STATIC_PICTURE)->SetWindowPos(0, x, y, rect.Size().cx, rect.Size().cy, NULL);*/
	CRect clientrect;
	GetClientRect(clientrect);
	GetDlgItem(IDC_STATIC_PICTURE)->MoveWindow(clientrect, TRUE);
	showimgsizex = clientrect.Size().cx;
	showimgsizey = clientrect.Size().cy;
}


int CinterferomeryDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CMFCMenuBar       m_wndMenuBar;
	CMenu menu;       // 菜单（包含主菜单栏和子菜单）   
					  //CMenu *pSubMenu;  // 右键菜单   

					  // 加载菜单资源到menu对象   
	menu.LoadMenu(IDR_MENU1);
	
	// 因为右键菜单是弹出式菜单，不包含主菜单栏，所以取子菜单   
	//pSubMenu = menu.GetSubMenu(0);
	//HMENU hmenu=
	//CMenu* pMainMenu = NULL;
	
	//menu.SetBackgroundColor(RGB(255, 0, 0));
	HMENU hmehu = menu.GetSafeHmenu();
	//CMFCMenuBar m_wndMenuBar;
	//CMenu *mainMenu = CMenu::FromHandle(m_wndMenuBar.GetDefaultMenu());
	//	LOGFONT m_lf; 
	//	memset(&m_lf, 0, sizeof(LOGFONT));       // zero out structure<br />
	//m_lf.lfHeight = 22;                      // request a 12-pixel-height font<br />
	//_tcsncpy_s(m_lf.lfFaceName, LF_FACESIZE,_T("宋体"), 17); 
	//	m_wndMenuBar.SetMenuFont(&m_lf); 


	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}
	   //m_wndMenuBar.CreateFromMenu(hmehu, TRUE, TRUE);
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	
	LOGFONT m_lf;
	memset(&m_lf, 0, sizeof(LOGFONT));

	m_lf.lfHeight = 26;
	m_lf.lfWeight = 700;//设置字体为粗体（一般为400，粗体为700）  

	_tcsncpy_s(m_lf.lfFaceName, LF_FACESIZE, _T("Arial"), 7); //字符拷贝函数，使用的如果是UNICODE编码，则采用wcscpy_s()函数，如果是多字节编码，则采用strcpy_s()函数 ,m_lf.lfFaceName设置字体名称  
	m_wndMenuBar.SetMenuFont(&m_lf);
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);

	return 0;
}


void CinterferomeryDlg::OnStaticAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::aperModel==2)
	{
		TaskDispose::staticanalysis = true;
	}
	//Mat tempPtMap = 255*TaskDispose::PtMap;
	//imshow("ptmap", tempPtMap);
}
//LRESULT CinterferomeryDlg::StaticAnaysis(WPARAM wParam, LPARAM lParam)
bool CinterferomeryDlg::StaticAnaysis()
{
	//CvFont font;
	//cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 1, 8);

	//Mat imgsrc = imread("33.bmp", IMREAD_COLOR);
	//int i = imgsrc.depth();
	//int j = imgsrc.channels();
	//int k = imgsrc.rows;
	//int l = imgsrc.cols;
	Mat img;
	if (TaskDispose::aperModel==1)
	{
		img = waitforprocframe;
	}
	else
	{
		Mat tempwaitforprocframe;
		tempwaitforprocframe = waitforprocframe(Rect(TaskDispose::origin.x, TaskDispose::origin.y, 512, 512));
		resize(tempwaitforprocframe, img, Size(128, 128), 0, 0, INTER_LINEAR);
	}
	
	ucharPtMap = new unsigned char[LENGTH][LENGTH];
	m_flag = new unsigned char[LENGTH][LENGTH];
	m_Type = PLANE;
	m_bScanWay = TRUE;
	m_measureBand = BAND1;
	Mat grayimg;
	cv::cvtColor(img, grayimg, CV_BGR2GRAY);//原始图片转换为灰度图
	//int m = grayimg.channels();


	int(*SumBuf)[LENGTH];
	SumBuf = new int[LENGTH][LENGTH];
	Image = new unsigned char[LENGTH][LENGTH];
	WavePhase = new int[LENGTH][LENGTH];
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			WavePhase[i][j] = NON;

	for (int k = 0; k < TaskDispose::length; k++)
		for (int l = 0; l < TaskDispose::length; l++)
		{
			Image[k][l] = (int)grayimg.at<uchar>(k, l);
		}
	for (int k = 0; k < TaskDispose::length; k++)
		for (int l = 0; l < TaskDispose::length; l++)
		{
			SumBuf[k][l] = Image[l][k];
		}
	for (int k = 0; k < 2; k++)
	{
		for (int i = 1; i < TaskDispose::length - 1; i++)
			for (int j = 1; j < TaskDispose::length - 1; j++)
			{
				int t = SumBuf[i - 1][j - 1] + SumBuf[i - 1][j] + SumBuf[i - 1][j + 1];
				t += SumBuf[i][j - 1] + SumBuf[i][j] * 8 + SumBuf[i][j + 1];
				t += SumBuf[i + 1][j - 1] + SumBuf[i + 1][j] + SumBuf[i + 1][j + 1];
				Image[i][j] = (unsigned char)((double)t / (double)16);
			}

		for (int i = 0; i < TaskDispose::length; i++)
			for (int j = 0; j < TaskDispose::length; j++)
				SumBuf[i][j] = Image[i][j];
	}


	//imshow("grayimg3.bmp", img);
	Mat tempPtMap = TaskDispose::PtMap;
	tempPtMap = TaskDispose::PtMap;
	resize(tempPtMap, m_PtMap, Size(128, 128), 0, 0, INTER_LINEAR);

	//imshow("PtMap", TaskDispose::PtMap*255);
	//m_PtMap = CreateEffectPtMap();//创建点阵图
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			ucharPtMap[j][i] = m_PtMap.data[i*TaskDispose::length + j];
	//multiply(m_PtMap, grayimg, grayimg, 1, -1);

	m_BWMap = BWinterferogram();//干涉图二值化
								//imshow("m_PtMap", m_PtMap);
								//imshow("m_BWMap1", m_BWMap);
								//Mat element1 = getStructuringElement(MORPH_RECT, Size(1, 1));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));
	//Mat element3 = getStructuringElement(MORPH_RECT, Size(3, 3));
	//erode(m_BWMap, m_BWMap, element2);//腐蚀，去除孤立亮点
	//dilate(m_BWMap, m_BWMap, element2);//膨胀，去除孤立黑点
	//erode(m_BWMap, m_BWMap, element2);//腐蚀，去除孤立亮点
	BWPreprocess(m_BWMap, m_PtMap);//去除孤立亮点，黑点，耗时90ms
	//imshow("m_BWMap", m_BWMap);

	NoiseRemoveInterferogram(Image);
	//Mat lowpassimg;
	//NoiseRemoveInterferogram(grayimg, lowpassimg);
	BLOCKINF*	block = new BLOCKINF[BLOCKNUMMAX];
	//Mat image(512, 512, CV_8UC1, cv::Scalar::all(0));
	int RetryNumber = 1;
	int	blocknum;
	//m_BWMap.copyTo(image);
	unsigned char(*tempdata)[LENGTH] = new unsigned char[LENGTH][LENGTH];
	for (int i = 0; i < TaskDispose::length; i++)
		for (int j = 0; j < TaskDispose::length; j++)
			tempdata[i][j] = m_BWMap.data[(i)*TaskDispose::length + j];

	//imshow("image", image);
	DWORD starttime, endtime;
	starttime = GetTickCount();



	for (int k = 0; k < RetryNumber; k++)
	{

		LableInterferogram(tempdata, block, blocknum);

		/*for (int i = 0; i < LENGTH; i++)
		for (int j = 0; j < LENGTH; j++)
		image.data[i*LENGTH + j] = tempdata[j][i]*50;*/
		//Mat showimage;
		//resize(image, showimage, Size(180, 180));
		//cv::imshow("sign", image);

		if (blocknum == BLOCKNUMMAX - 1)
		{
			//MessageBox(_T("条纹数太多或条纹质量原因，无法处理"));
			delete[] ucharPtMap;
			return false;
		}

		InitialBlockInf(block, blocknum);
		for (int i = 1; i <= blocknum; i++)
		{
			FindNeighbour(tempdata, block, i);
		}
		DeleteUnwantedBlock(tempdata, block, blocknum);
		//int result = sizeof(block);

		if (IsMaxNeighbourNumGreaterThanTwo(block, blocknum))
		{
			if (TaskDispose::aperType == 2 && RetryNumber > 0) break;
			//if (k == RetryNumber - 1)
			//{
			//	bSuccess = FALSE;
			//	break;
			//}
			//针对相邻数超过2的情况，对二值化图象的适当区域作细化处理
			ReProcessBWMap(tempdata, block, blocknum);
		}
		else break;
	}

	if (!OrderInterferogram(tempdata, block, blocknum))
	{
		//MessageBox(_T("条纹质量原因，无法处理"));
		return false;
	}

	endtime = GetTickCount();
	//showtime(starttime, endtime);
	//建立符号图
	unsigned char(*sign)[LENGTH];
	sign = new unsigned char[LENGTH][LENGTH];

	DefineSignMap(tempdata, block, blocknum, sign);
	/*for (int i = 0; i < LENGTH; i++)
	for (int j = 0; j < LENGTH; j++)
	image.data[i*LENGTH + j] = Image[LENGTH-1-j][i] ;
	imshow("image", image);*/

	//确定干涉图中明暗细条纹的位置
	unsigned char(*fringe)[LENGTH];
	fringe = new unsigned char[LENGTH][LENGTH];
	DefineThinFringe(tempdata, block, blocknum, sign, fringe);

	//计算位相
	CalculatePhase(sign, fringe);
	//cv::namedWindow("WavePhase", CV_WINDOW_NORMAL);
	//Mat resizeimage;
	//resize(image, resizeimage,Size(256, 256));
	Cont();
	//for (int i = 0; i < LENGTH; i++)
	//	for (int j = 0; j < LENGTH; j++)
	//		image.data[i*LENGTH + j] = WavePhase[j][i];
	//cv::imshow("Image", image);
	Connect();
	Sms();
	m_bScanWay = FALSE;
	NoiseRemove();
	m_bScanWay = TRUE;
	for (int i = 0; i < 6; i++)
		Smooth();

	/*for (int i = 0; i < LENGTH; i++)
		for (int j = 0; j < LENGTH; j++)
			image.data[i*LENGTH + j] = tempdata[i][j];*/
	delete[] tempdata;
	//imshow("result", image);

	/*IplImage *MFCimage = NULL;
	MFCimage = &IplImage(grayimg);
	cvPutText(MFCimage, "PV", cvPoint(60, 35), &font, cvScalar(255, 255, 255));
	cvPutText(MFCimage, "0.052Wave", cvPoint(10, 60), &font, cvScalar(255, 255, 255));
	cvPutText(MFCimage, "POWER", cvPoint(360, 35), &font, cvScalar(255, 255, 255));
	cvPutText(MFCimage, "0.052Wave", cvPoint(330, 60), &font, cvScalar(255, 255, 255));
	DrawPicToHDC(MFCimage, IDC_STATIC_PICTURE);*/


	int pts = GetPTS();
	float pv = GetPV();
	if (pv < 0.0001 || pv>20)
	{
		return false;
	}
	float rms = GetRms();
	float em = GetEms();
	float Nvalue = GetN();
	float deltan = GetDeltN(0);
	//CString str;
	//str.Format(_T("pts%d pv%f rms%f em%f Nvalue%f deltan%f"), pts, pv, rms, em, Nvalue, deltan);
	//MessageBox(str);
	float zernikecoff=CalculateZern();
	PV = pv*0.5;
	POWER = zernikecoff;
	PVrms = rms;


	delete[] ucharPtMap;
	delete[] WavePhase;
	delete[] m_flag;
	delete[] Image;
	delete block;
	delete[] fringe;
	delete[] sign;
	return true;
}
// Get center point and other parameters 
void	CinterferomeryDlg::Center(int * w, int size,float& center_x, float& center_y,float& r, int& istart, int& jstart, int& len)
{
	int i, j, imax = 0, imin = 10000, jmax = 0, jmin = 10000;
	float	rr;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			if ((*(w + i*size + j)) != NON) {
				if (i > imax) imax = i;
				if (i < imin) imin = i;
				if (j > jmax) jmax = j;
				if (j<jmin) jmin = j;
			}
	istart = imin;
	jstart = jmin;
	len = ((jmax - jmin)>(imax - imin) ? (jmax - jmin) : (imax - imin)) + 1;
	center_x = (float)imin + ((float)imax - (float)imin) / (float)2.0;
	center_y = (float)jmin + ((float)jmax - (float)jmin) / (float)2.0;
	r = 0.0;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			if ((*(w + i*size + j)) != NON) {
				rr = (float)sqrt((double)(((float)i - center_x)*
					((float)i - center_x) +
					((float)j - center_y)*
					((float)j - center_y)));
				if (rr > r) r = rr;
			}
}

float CinterferomeryDlg::CalculateZern()
{
	if (GetPTS() < 5) return 0;
	int* w = (int *)GlobalAllocPtr(GHND,
		(DWORD)TaskDispose::length / 4 * (DWORD)TaskDispose::length / 4 * (DWORD)sizeof(int));
	if (!w)
	{
		//AfxMessageBox(
			//"Unable to allocate memory for Zernike Coefficients calculate");
		return 0;
	}


	for (int i = 0; i < TaskDispose::length / 4; i++)
		for (int j = 0; j < TaskDispose::length / 4; j++)
			*(w + i*TaskDispose::length / 4 + j) = WavePhase[i * 4][j * 4];


	/*get the center ,r,ect.*/
	float	center_x, center_y, r;
	int		istart, jstart, len;
	Center(w, TaskDispose::length / 4, center_x, center_y, r, istart, jstart, len);

	/*get space*/
	struct polar * r_t = (struct polar *)GlobalAllocPtr(GHND,
		(DWORD)len*(DWORD)len*(DWORD)sizeof(struct polar));
	if (!r_t)
	{
		//AfxMessageBox(
			//"Unable to allocate memory for Zernike Coefficients calculate");
		return 0;
	}

	int* ww = (int *)GlobalAllocPtr(GHND,
		(DWORD)len*(DWORD)len*(DWORD)sizeof(int));
	if (!ww)
	{
		//AfxMessageBox(
			//"Unable to allocate memory for Zernike Coefficients calculate");
		return 0;
	}


	/*transfer of coordination*/
	for (int i = 0; i < len; i++)
		for (int j = 0; j < len; j++) {
			int	t1, t2;
			t1 = istart + i;
			t2 = jstart + j;
			if ((istart + i >= TaskDispose::length / 4) || (jstart + j >= TaskDispose::length / 4)) {
				*(ww + i*len + j) = NON;
				continue;
			}
			/*if(w[i+istart][j+jstart]!=NON){*/
			if ((*(w + t1*TaskDispose::length / 4 + t2)) != NON) {
				float x = ((float)i + istart - center_x) / r;
				float y = ((float)j + jstart - center_y) / r;
				float rr = (float)sqrt((double)(x*x + y*y));
				float theta = 0.0;
				if (x == 0.0) theta = y > 0.0 ? (float)(PI / 2) : (float)(-PI / 2);
				else if (y == 0.0) theta = x>0.0 ? (float)0.0 : (float)PI;
				else {
					if (((x<0.0) && (y>0.0)) || ((x < 0.0) && (y < 0.0))) {
						theta = (float)PI;   y = (float)(-y);
					}
					theta += (float)asin((double)(y / rr));
				}
				(r_t + i*len + j)->r = rr;
				(r_t + i*len + j)->theta = theta;
				/* *(ww+i*len+j)= w[i+istart][j+jstart];*/
				*(ww + i*len + j) = *(w + t1*TaskDispose::length / 4 + t2);
			}
			else {
				*(ww + i*len + j) = NON;
			}
		}

	/*obtain zernike coefficence*/
	zernike(ww, len, r_t, m_zern4, 4);

	zernike(ww, len, r_t, m_zernCoef, 36);
	float div;
	//if (TaskDispose::aperModel==1)
	//{
		div = 256.0;
	//}
	//else
	//{
		//div = 1024.0;
	//}
	/* Piston */
	Sv[0] = m_zernCoef[0] - m_zernCoef[3] + m_zernCoef[8] - m_zernCoef[15] + m_zernCoef[24] - m_zernCoef[35];

	/* Focus */
	Sv[1] = 2 * m_zernCoef[3] - 6 * m_zernCoef[8] + 12 * m_zernCoef[15] - 20 * m_zernCoef[24] + 30 * m_zernCoef[35];

	/* Y tilt */
	Sv[2] = m_zernCoef[2] - 2 * m_zernCoef[7] + 3 * m_zernCoef[14] - 4 * m_zernCoef[23] + 5 * m_zernCoef[34];

	/* X tilt */
	Sv[3] = m_zernCoef[1] - 2 * m_zernCoef[6] + 3 * m_zernCoef[13] - 4 * m_zernCoef[22] + 5 * m_zernCoef[33];

	/* Sphereical */
	/* 3rd order */
	Sv[4] = 6 * m_zernCoef[8] - 30 * m_zernCoef[15] + 90 * m_zernCoef[24] - 210 * m_zernCoef[35];
	/* 5th order */
	Sv[5] = 20 * m_zernCoef[15] - 140 * m_zernCoef[24] + 560 * m_zernCoef[35];
	/* 7th order */
	Sv[6] = 70 * m_zernCoef[24] - 630 * m_zernCoef[35];
	/* 9th order */
	Sv[7] = 252 * m_zernCoef[35];

	/* Y coma */
	/* 3rd order */
	Sv[8] = 3 * m_zernCoef[7] - 12 * m_zernCoef[14] + 30 * m_zernCoef[23] - 60 * m_zernCoef[34];
	/* 5th order */
	Sv[9] = 10 * m_zernCoef[14] - 60 * m_zernCoef[23] + 210 * m_zernCoef[34];
	/* 7th order */
	Sv[10] = 35 * m_zernCoef[23] - 280 * m_zernCoef[34];
	/* 9th order */
	Sv[11] = 126 * m_zernCoef[34];

	/* X coma */
	/* 3rd order */
	Sv[12] = 3 * m_zernCoef[6] - 12 * m_zernCoef[13] + 30 * m_zernCoef[22] - 60 * m_zernCoef[33];
	/* 5th order */
	Sv[13] = 10 * m_zernCoef[13] - 60 * m_zernCoef[22] + 210 * m_zernCoef[33];
	/* 7th order */
	Sv[14] = 35 * m_zernCoef[22] - 280 * m_zernCoef[33];
	/* 9th order */
	Sv[15] = 126 * m_zernCoef[33];

	/* 45 degree Asitig */
	/* 3rd order */
	Sv[16] = m_zernCoef[5] - 3 * m_zernCoef[12] + 6 * m_zernCoef[21] - 10 * m_zernCoef[32];
	/* 5th order */
	Sv[17] = 4 * m_zernCoef[12] - 20 * m_zernCoef[21] + 60 * m_zernCoef[32];
	/* 7th order */
	Sv[18] = 15 * m_zernCoef[21] - 105 * m_zernCoef[32];
	/* 9th order */
	Sv[19] = 56 * m_zernCoef[32];

	/* 0 degree Asitig */
	/* 3rd order */
	Sv[20] = m_zernCoef[4] - 3 * m_zernCoef[11] + 6 * m_zernCoef[20] - 10 * m_zernCoef[31];
	/* 5th order */
	Sv[21] = 4 * m_zernCoef[11] - 20 * m_zernCoef[20] + 60 * m_zernCoef[31];
	/* 7th order */
	Sv[22] = 15 * m_zernCoef[20] - 105 * m_zernCoef[31];
	/* 9th order */
	Sv[23] = 56 * m_zernCoef[31];

	/* 30 degree Tri */
	/* 5th order */
	Sv[24] = m_zernCoef[10] - 4 * m_zernCoef[19] + 10 * m_zernCoef[30];
	/* 7th order */
	Sv[25] = 5 * m_zernCoef[19] - 30 * m_zernCoef[30];
	/* 9th order */
	Sv[26] = 21 * m_zernCoef[30];

	/* 0 degree Tri */
	/* 5th order */
	Sv[27] = m_zernCoef[9] - 4 * m_zernCoef[18] + 10 * m_zernCoef[29];
	/* 7th order */
	Sv[28] = 5 * m_zernCoef[18] - 30 * m_zernCoef[29];
	/* 9th order */
	Sv[29] = 21 * m_zernCoef[29];

	/* 22.5 degree Quad */
	/* 7th order */
	Sv[30] = m_zernCoef[17] - 5 * m_zernCoef[28];
	/* 9th order */
	Sv[31] = 6 * m_zernCoef[28];

	/* 0 degree Quad */
	/* 7th order */
	Sv[32] = m_zernCoef[16] - 5 * m_zernCoef[27];
	/* 9th order */
	Sv[33] = 6 * m_zernCoef[27];

	/* 18 degree Penta */
	/* 9th order */
	Sv[34] = m_zernCoef[26];

	/* 0 degree Penta */
	/* 9th order */
	Sv[35] = m_zernCoef[25];
	for (int i = 0; i < 36; i++)
	{
		Sv[i]=Sv[i] / div;
	}
		
	//float zerncof = (2 * m_zernCoef[3] - 6 * m_zernCoef[8] + 12 * m_zernCoef[15] - 20 * m_zernCoef[24] + 30 * m_zernCoef[35])/div;
	float zerncof, zerncof1, zerncof2;
	zerncof1 = 2 * m_zernCoef[3] - 6 * m_zernCoef[8] + sqrt(m_zernCoef[4]* m_zernCoef[4] + m_zernCoef[5]* m_zernCoef[5]);
	zerncof2 = 2 * m_zernCoef[3] - 6 * m_zernCoef[8] - sqrt(m_zernCoef[4] * m_zernCoef[4] + m_zernCoef[5] * m_zernCoef[5]);
	zerncof = min(abs(zerncof1), abs(zerncof2)) / div;
	/*if (zerncof==abs(zerncof1))
	{
		zerncof = zerncof1 / div;
	}
	else
	{
		zerncof = zerncof2/ div;
	}*/
	GlobalFreePtr(w);
	GlobalFreePtr(r_t);
	GlobalFreePtr(ww);
	return zerncof;
}
void CinterferomeryDlg::OnSigle()
{
	// TODO: 在此添加命令处理程序代码
	m_listBox.ResetContent();
	TaskDispose::aperModel = 0;
	TaskDispose::length = 128;
	//m_StatBar->SetText(_T("单片自动选择孔径模式"), 0, 0);
	CRect clientrect;
	GetClientRect(clientrect);
	GetDlgItem(IDC_STATIC_PICTURE)->MoveWindow(clientrect, TRUE);
	showimgsizex = clientrect.Size().cx;
	showimgsizey = clientrect.Size().cy;
	//CRect  rect;
	//GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(rect);
	//int x = clientrect.Size().cx*0.15;
	//int y = clientrect.Size().cy*0.15;
	//GetDlgItem(IDC_STATIC_PICTURE)->SetWindowPos(0, x, y, rect.Size().cx, rect.Size().cy, NULL);
	//GetDlgItem(IDC_STATIC_PICTURE)->SetWindowPos(0, 0, 0, clientrect.Size().cx, clientrect.Size().cy, NULL);
	
	//CRect Rect;
	//GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(Rect);
	//ScreenToClient(Rect);
	//CPoint OldTLPoint, TLPoint; //左上角
	//CPoint OldBRPoint, BRPoint; //右下角
	//OldTLPoint = Rect.TopLeft();
	//TLPoint.x = 0;
	//TLPoint.y = 0;
	//OldBRPoint = Rect.BottomRight();
	//BRPoint.x = long(OldBRPoint.x *1.5);
	//BRPoint.y = long(OldBRPoint.y *1.5);
	//Rect.SetRect(TLPoint, BRPoint);
	
	//GetDlgItem(IDC_STATIC_PICTURE)->MoveWindow(clientrect, TRUE);

}


void CinterferomeryDlg::OnMulty()
{
	// TODO: 在此添加命令处理程序代码
	TaskDispose::aperModel = 1;
	TaskDispose::length = 128;
	//m_StatBar->SetText(_T("多片自动选择孔径模式"), 0, 0);
	CRect clientrect;
	GetClientRect(clientrect);
	CRect  rect;
	GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(rect);
	int x = clientrect.Size().cx*0.01;
	int y = clientrect.Size().cy*0.15;
	
	int cy = clientrect.Size().cy*0.64;
	int cx = cy*1.33;
	GetDlgItem(IDC_STATIC_PICTURE)->SetWindowPos(0, x, y, cx, cy, NULL);
	showimgsizex = cx;
	showimgsizey = cy;
}


void CinterferomeryDlg::OnPvPv()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos==0)
	{
		TaskDispose::SLData = 0;
	}
	else if (TaskDispose::RButtonPos==1)
	{
		TaskDispose::SRData = 0;
	}
}


void CinterferomeryDlg::OnPvPvrms()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 1;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 1;
	}
}


void CinterferomeryDlg::OnPower()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 2;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 2;
	}
}


void CinterferomeryDlg::OnN()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 3;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 3;
	}
}


void CinterferomeryDlg::OnDeltan()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 4;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 4;
	}
}


void CinterferomeryDlg::Onaperture()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 5;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 5;
	}
}


void CinterferomeryDlg::OnAsia()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 6;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 6;
	}
}


void CinterferomeryDlg::OnPvFr()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 0;
		TaskDispose::SLUnit = 0;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 0;
		TaskDispose::SRUnit = 0;
	}
}


void CinterferomeryDlg::OnPvWave()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 0;
		TaskDispose::SLUnit = 1;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 0;
		TaskDispose::SRUnit = 1;
	}
}


void CinterferomeryDlg::OnPvum()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 0;
		TaskDispose::SLUnit = 2;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 0;
		TaskDispose::SRUnit = 2;
	}
}


void CinterferomeryDlg::OnPvNm()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 0;
		TaskDispose::SLUnit = 3;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 0;
		TaskDispose::SRUnit = 3;
	}
}


void CinterferomeryDlg::OnPvrmsFr()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 1;
		TaskDispose::SLUnit = 0;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 1;
		TaskDispose::SRUnit = 0;
	}
}


void CinterferomeryDlg::OnPvrmsWave()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 1;
		TaskDispose::SLUnit = 1;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 1;
		TaskDispose::SRUnit = 1;
	}
}


void CinterferomeryDlg::OnPvrmsum()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 1;
		TaskDispose::SLUnit = 2;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 1;
		TaskDispose::SRUnit = 2;
	}
}


void CinterferomeryDlg::OnPvrmsNm()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 1;
		TaskDispose::SLUnit = 3;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 1;
		TaskDispose::SRUnit = 3;
	}
}


void CinterferomeryDlg::OnPowerFr()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 2;
		TaskDispose::SLUnit = 0;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 2;
		TaskDispose::SRUnit = 0;
	}
}


void CinterferomeryDlg::OnPowerWave()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 2;
		TaskDispose::SLUnit = 1;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 2;
		TaskDispose::SRUnit = 1;
	}
}


void CinterferomeryDlg::OnPowerum()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 2;
		TaskDispose::SLUnit = 2;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 2;
		TaskDispose::SRUnit = 2;
	}
}


void CinterferomeryDlg::OnPowerNm()
{
	// TODO: 在此添加命令处理程序代码
	if (TaskDispose::RButtonPos == 0)
	{
		TaskDispose::SLData = 2;
		TaskDispose::SLUnit = 3;
	}
	else if (TaskDispose::RButtonPos == 1)
	{
		TaskDispose::SRData = 2;
		TaskDispose::SRUnit = 3;
	}
}


//void CinterferomeryDlg::OnRButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	HWND hwnd;
//	hwnd = this->m_hWnd;
//	if ((point.x<900 && point.x>700 && point.y<500 && point.y>0))
//	{
//		CMenu menu;
//		menu.LoadMenu(IDR_MENU3);        //CG_IDR_POPUP_TYPING_TEST_VIEW为菜单ID
//		CMenu* pPopup = menu.GetSubMenu(0);
//		ASSERT(pPopup != NULL);
//		//ClientToScreen(this->m_hWnd, &point);
//		this->ClientToScreen(&point);
//		TaskDispose::RButtonPos = 0;
//		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
//		return;
//	}
//	CDialogEx::OnRButtonUp(nFlags, point);
//}



void CinterferomeryDlg::On32779()
{
	// TODO: 在此添加命令处理程序代码
	InfSet dlg;
	dlg.DoModal();
}

CString CinterferomeryDlg::DrawResult(Mat & frame)
{
	CString returecstr;
	returecstr = "";
	
	String labelleft,labelright,strleft, strright,strleftunit,strrightunit;
	char t[256];
	if (TaskDispose::SLData==0)
	{
		labelleft = "PV";
	}
	else if (TaskDispose::SLData==1)
	{
		labelleft = "RMS";
	}
	else if (TaskDispose::SLData == 2)
	{
		labelleft = "POWER";
	}
	else if (TaskDispose::SLData == 3)
	{
		labelleft = "N";
	}
	else if (TaskDispose::SLData == 4)
	{
		labelleft = "ΔN";
	}
	else if (TaskDispose::SLData == 5)
	{
		labelleft = "光圈";
	}
	else if (TaskDispose::SLData == 6)
	{
		labelleft = "亚斯/本";
	}
	
	if (TaskDispose::SLUnit==0)
	{
		strleftunit = "fr";
	}
	else if (TaskDispose::SLUnit==1)
	{
		strleftunit = "wave";
	}
	else if (TaskDispose::SLUnit == 2)
	{
		strleftunit = "μm";
	}
	else if (TaskDispose::SLUnit == 3)
	{
		strleftunit = "nm";
	}
	
	if (TaskDispose::SLData==0||TaskDispose::SLData==4||TaskDispose::SLData==6)
	{
		if (TaskDispose::SLUnit==0)
		{
			sprintf_s(t, "%5.2f", PV/(float)2.0);
		}
		else if (TaskDispose::SLUnit==1)
		{
			sprintf_s(t, "%5.2f", PV );
		}
		else if (TaskDispose::SLUnit==2)
		{
			sprintf_s(t, "%5.2f", PV*(float)0.6328);
		}
		else if (TaskDispose::SLUnit==3)
		{
			sprintf_s(t, "%5.2f", PV*(float)632.8);
		}	
	}
	else if (TaskDispose::SLData == 2 || TaskDispose::SLData == 3 || TaskDispose::SLData == 5)
	{
		if (TaskDispose::SLUnit == 0)
		{
			sprintf_s(t, "%5.2f", POWER / (float)2.0);
		}
		else if (TaskDispose::SLUnit == 1)
		{
			sprintf_s(t, "%5.2f", POWER);
		}
		else if (TaskDispose::SLUnit == 2)
		{
			sprintf_s(t, "%5.2f", POWER*(float)0.6328);
		}
		else if (TaskDispose::SLUnit == 3)
		{
			sprintf_s(t, "%5.2f", POWER*(float)632.8);
		}
	}
	else if (TaskDispose::SLData==1)
	{
		if (TaskDispose::SLUnit == 0)
		{
			sprintf_s(t, "%5.2f", PVrms / (float)2.0);
		}
		else if (TaskDispose::SLUnit == 1)
		{
			sprintf_s(t, "%5.2f", PVrms);
		}
		else if (TaskDispose::SLUnit == 2)
		{
			sprintf_s(t, "%5.2f", PVrms*(float)0.6328);
		}
		else if (TaskDispose::SLUnit == 3)
		{
			sprintf_s(t, "%5.2f", PVrms*(float)632.8);
		}
	}
	strleft = t;
	strleft += strleftunit;

	if (TaskDispose::SRData == 0)
	{
		labelright = "PV";
	}
	else if (TaskDispose::SRData == 1)
	{
		labelright = "RMS";
	}
	else if (TaskDispose::SRData == 2)
	{
		labelright = "POWER";
	}
	else if (TaskDispose::SRData == 3)
	{
		labelright = "N";
	}
	else if (TaskDispose::SRData == 4)
	{
		labelright = "ΔN";
	}
	else if (TaskDispose::SRData == 5)
	{
		labelright = "光圈";
	}
	else if (TaskDispose::SRData == 6)
	{
		labelright = "亚斯/本";
	}


	if (TaskDispose::SRUnit == 0)
	{
		strrightunit = "fr";
	}
	else if (TaskDispose::SRUnit == 1)
	{
		strrightunit = "wave";
	}
	else if (TaskDispose::SRUnit == 2)
	{
		strrightunit = "μm";
	}
	else if (TaskDispose::SRUnit == 3)
	{
		strrightunit = "nm";
	}
	if (TaskDispose::SRData == 0 || TaskDispose::SRData == 4 || TaskDispose::SRData == 6)
	{
		if (TaskDispose::SRUnit == 0)
		{
			sprintf_s(t, "%5.2f", PV / (float)2.0);
		}
		else if (TaskDispose::SRUnit == 1)
		{
			sprintf_s(t, "%5.2f", PV);
		}
		else if (TaskDispose::SRUnit == 2)
		{
			sprintf_s(t, "%5.2f", PV*(float)0.6328);
		}
		else if (TaskDispose::SRUnit == 3)
		{
			sprintf_s(t, "%5.2f", PV*(float)632.8);
		}
	}
	else if (TaskDispose::SRData == 2 || TaskDispose::SRData == 3 || TaskDispose::SRData == 5)
	{
		if (TaskDispose::SRUnit == 0)
		{
			sprintf_s(t, "%5.2f", POWER / (float)2.0);
		}
		else if (TaskDispose::SRUnit == 1)
		{
			sprintf_s(t, "%5.2f", POWER);
		}
		else if (TaskDispose::SRUnit == 2)
		{
			sprintf_s(t, "%5.2f", POWER*(float)0.6328);
		}
		else if (TaskDispose::SRUnit == 3)
		{
			sprintf_s(t, "%5.2f", POWER*(float)632.8);
		}
	}
	else if (TaskDispose::SRData == 1)
	{
		if (TaskDispose::SRUnit == 0)
		{
			sprintf_s(t, "%5.2f", PVrms / (float)2.0);
		}
		else if (TaskDispose::SRUnit == 1)
		{
			sprintf_s(t, "%5.2f", PVrms);
		}
		else if (TaskDispose::SRUnit == 2)
		{
			sprintf_s(t, "%5.2f", PVrms*(float)0.6328);
		}
		else if (TaskDispose::SRUnit == 3)
		{
			sprintf_s(t, "%5.2f", PVrms*(float)632.8);
		}
	}
	strright = t;
	strright += strrightunit;
	//strleft += "Wave";
	//sprintf_s(t, "%5.2f", POWER);
	//strright = t;
	//strright += "Wave";

	//putText(frame, labelleft, cvPoint(50, 35), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 200, 200), 2, 8, 0);
	//putText(frame, strleft, cvPoint(10, 70), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255), 2, 8, 0);
	//putText(frame, labelright, cvPoint(530, 35), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 200, 200), 2, 8, 0);
	//putText(frame, strright, cvPoint(480, 70), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255), 2, 8, 0);	
	
	if (TaskDispose::aperModel==0||TaskDispose::aperModel==2)
	{
		const char* showtext = labelleft.c_str();
		putTextZH(frame, showtext, cvPoint(60, 15), Scalar(0, 200, 200), 30, "楷体", false, false);
		showtext = strleft.c_str();
		putTextZH(frame, showtext, cvPoint(6, 50), Scalar(0, 255, 255), 30, "楷体", false, false);
		showtext = labelright.c_str();
		putTextZH(frame, showtext, cvPoint(600, 15), Scalar(0, 200, 200), 30, "楷体", false, false);
		showtext = strright.c_str();
		putTextZH(frame, showtext, cvPoint(550, 50), Scalar(0, 255, 255), 30, "楷体", false, false);
		return returecstr;
	}
	else if (TaskDispose::aperModel==1)
	{
		//returecstr = labelleft.c_str()+strleft.c_str()+labelright.c_str()+strright.c_str();
		//returecstr.Format("%s%s%s%s", labelleft.c_str(), strleft.c_str(), labelright.c_str(), strright.c_str());
		CString str1(labelleft.c_str());
		CString str2(strleft.c_str());
		CString str3(labelright.c_str());
		CString str4(strright.c_str());
		returecstr = _T(" ")+str1 + _T(" ") + str2 + _T("  ") + str3 + _T(" ") + str4;
		return returecstr;
	}
}

void CinterferomeryDlg::On32778()
{
	// TODO: 在此添加命令处理程序代码
	cvWaitKey(100);
	HWND hwnd = ::GetDesktopWindow();//截整个屏幕，用从这往下4句
	HDC hdc = ::GetDC(hwnd);
	CDC dc;
	dc.Attach(hdc);
	CRect rc;
	GetClientRect(&rc);//只截对话框，用这句
	ClientToScreen(&rc);
	//GetWindowRect(FindWindow(NULL, L"计算器"), &rect);
	//rc.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));//截整个屏幕，用这句
	int iBitPerPixel = dc.GetDeviceCaps(BITSPIXEL);
	int iWidth = rc.Width();
	int iHeight = rc.Height();
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap memBitmap, *oldBitmap;

	memBitmap.CreateCompatibleBitmap(&dc, iWidth, iHeight);
	oldBitmap = memDC.SelectObject(&memBitmap);

	memDC.BitBlt(0, 0, iWidth, iHeight, &dc, rc.TopLeft().x, rc.TopLeft().y, SRCCOPY);

	BITMAP bmp;
	memBitmap.GetBitmap(&bmp);

	FILE *fp;
	fopen_s(&fp,"test2.bmp", "wb");
	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(bih));
	bih.biBitCount = bmp.bmBitsPixel;
	bih.biCompression = BI_RGB;//表示不压缩
	bih.biHeight = bmp.bmHeight;
	bih.biPlanes = 1;//位平面数，必须为1
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;
	bih.biWidth = bmp.bmWidth;
	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;
	bfh.bfType = (WORD)0x4d42;//必须表示"BM"

	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);
	fwrite(&bih, 1, sizeof(bih), fp);

	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];
	GetDIBits(memDC.m_hDC, (HBITMAP)memBitmap.m_hObject, 0, iHeight, p, (LPBITMAPINFO)&bih, DIB_RGB_COLORS);
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);
	delete[] p;
	fclose(fp);

	memDC.SelectObject(oldBitmap);
	Mat image = imread("test2.bmp", IMREAD_COLOR);
	//imshow("showimg", image);
	Mat imgprint(image.rows+100, image.cols,image.type());
	imgprint.setTo(255);
	image.copyTo(imgprint(Rect(0, 0, image.cols, image.rows)));
	string  str1 = "公司：";
	string  str2 = "测试者：";
	string  str3 = "产品编号：";
	string  str4 = "日期时间：";
	string  resultstr1 = "上海埃旭光电";
	string  resultstr2 = "测试者：";
	string  resultstr3 = "产品编号：";
	string  resultstr4 = "日期时间：";
	const char* showtext = str1.c_str();
	putTextZH(imgprint, showtext, cvPoint(30, image.rows+20), Scalar(0, 0, 0), 30, "楷体", false, false);
	showtext = resultstr1.c_str();
	putTextZH(imgprint, showtext, cvPoint(130, image.rows + 20), Scalar(0, 0, 0), 30, "楷体", false, false);
	
	showtext = str2.c_str();
	putTextZH(imgprint, showtext, cvPoint(650, image.rows + 20), Scalar(0, 0, 0), 30, "楷体", false, false);
	//showtext = (char*)TaskDispose::tester.GetBuffer(TaskDispose::tester.GetLength()+1);
	
	CString cstrtext;
	cstrtext = TaskDispose::tester;
	string strtext(CW2A(cstrtext.GetString()));
	showtext = strtext.c_str();
	//strtext = cstrtext.GetString();
	//strncpy(showtext, TaskDispose::tester, TaskDispose::tester.GetLength());
	putTextZH(imgprint, showtext, cvPoint(780, image.rows + 20), Scalar(0, 0, 0), 30, "楷体", false, false);

	showtext = str3.c_str();
	putTextZH(imgprint, showtext, cvPoint(20, image.rows + 60), Scalar(0, 0, 0), 30, "楷体", false, false);
	cstrtext = TaskDispose::productID;
	string strtext3(CW2A(cstrtext.GetString()));
	showtext = strtext3.c_str();
	putTextZH(imgprint, showtext, cvPoint(180, image.rows + 60), Scalar(0, 0, 0), 30, "楷体", false, false);

	showtext = str4.c_str();
	putTextZH(imgprint, showtext, cvPoint(640, image.rows + 60), Scalar(0, 0, 0), 30, "楷体", false, false);
	CString str; //获取系统时间 　　
	CTime tm; tm = CTime::GetCurrentTime();
	str = tm.Format("%Y年%m月%d日%H时%M分%S秒");
	string strtext4(CW2A(str.GetString()));
	showtext = strtext4.c_str();
	putTextZH(imgprint, showtext, cvPoint(780, image.rows + 60), Scalar(0, 0, 0), 30, "楷体", false, false);
	//imshow("showprint", imgprint);
	strtext4 = strtext4 + ".jpg";
	//strFile = tm.Format("\\%Y年%m月%d日%H时%M分%S秒.xls");
	imwrite(strtext4, imgprint);
}


void CinterferomeryDlg::On32777()
{
	// TODO: 在此添加命令处理程序代码
	//CPrintFrame* pf = new CPrintFrame(this);
}


void CinterferomeryDlg::On32790()
{
	// TODO: 在此添加命令处理程序代码
	CAperAndFilt dlg;
	dlg.DoModal();
}


void CinterferomeryDlg::OnExcel()
{
	// TODO: 在此添加命令处理程序代码
	TCHAR *path = new TCHAR[MAX_PATH];
	ZeroMemory(path, MAX_PATH);
	// path == "d:\Project\Test\MFC\MFC\debug"
	GetCurrentDirectory(MAX_PATH, path);
	CString strPath;
	strPath.Format(_T("%s"), path);
	CString strFile; //获取系统时间 　
	CTime tm; tm = CTime::GetCurrentTime();
	strFile = tm.Format("\\%Y年%m月%d日%H时%M分%S秒.xls");
	strFile = strPath + strFile;
	//strFile = _T("E:\\") + strFile;
	//CString strFile = _T("E:\\Test.xls");
	COleVariant
		covTrue((short)TRUE),
		covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	CApplication   app;
	CWorkbooks   books;
	CWorkbook   book;
	CWorksheets   sheets;
	CWorksheet   sheet;
	CRange   range;
	CFont0   font;
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		MessageBox(_T("创建失败！"));
		return;
	}
	//Get   a   new   workbook.

	books = app.get_Workbooks();
	book = books.Add(covOptional);
	sheets = book.get_Worksheets();
	sheet = sheets.get_Item(COleVariant((short)1));
	int m_cols=4;
	int m_rows = 36;
	CString   colname;
	CString strTemp;
	for (int iCol = 0; iCol < m_cols; iCol++)//将列表的标题头写入EXCEL
	{

		GetCellName(1, iCol + 1, colname);
		range = sheet.get_Range(COleVariant(colname), COleVariant(colname));
		//pmyHeaderCtrl->GetItem(iCol, &hdi);
		//range.put_Value2(COleVariant(hdi.pszText));
		//int   nWidth = m_clcData.GetColumnWidth(iCol) / 6;
		//得到第iCol+1列  
		range.AttachDispatch(range.get_Item(_variant_t((long)(iCol + 1)), vtMissing).pdispVal, true);
		//设置列宽
		range.put_ColumnWidth(_variant_t((long)15));
	}
	range = sheet.get_Range(COleVariant(_T("A1 ")), COleVariant(colname));
	range.put_RowHeight(_variant_t((long)15));//设置行的高度
	font = range.get_Font();
	//font.put_Bold(covTrue);
	range.put_VerticalAlignment(COleVariant((short)-4108));//xlVAlignCenter   =   -4108
	COleSafeArray   saRet;
	DWORD   numElements[] = { m_rows,m_cols };       //5x2   element   array
	saRet.Create(VT_BSTR, 2, numElements);
	range = sheet.get_Range(COleVariant(_T("A1 ")), covOptional);
	range = range.get_Resize(COleVariant((short)m_rows), COleVariant((short)m_cols));
	long   index[2];
	range = sheet.get_Range(COleVariant(_T("A1 ")), covOptional);
	range = range.get_Resize(COleVariant((short)m_rows), COleVariant((short)m_cols));
	int iCol = 1;
	for (int iRow = 1; iRow <= m_rows; iRow++)//将列表内容写入EXCEL
	{
		/*for (int iCol = 1; iCol <= m_cols; iCol++)
		{*/
			index[0] = iRow - 1;
			index[1] = iCol - 1;
			CString   szTemp;
			CString rowstr;
			//rowstr.Format("%d", iRow);
			rowstr.Format(_T("%d"), iRow);
			CString str1 = _T("zerncoffe[");
			CString str2 = _T("]");
			szTemp = str1 + rowstr + str2;
			BSTR   bstr = szTemp.AllocSysString();
			saRet.PutElement(index, bstr);
			SysFreeString(bstr);
		//}
	}
	iCol = 2;
	for (int iRow = 1; iRow <= m_rows; iRow++)//将列表内容写入EXCEL
	{
		/*for (int iCol = 1; iCol <= m_cols; iCol++)
		{*/
		index[0] = iRow - 1;
		index[1] = iCol - 1;
		CString   szTemp;
		//rowstr.Format("%d", iRow);
		szTemp.Format(_T("%5.2f"), m_zernCoef[iRow-1]);
		BSTR   bstr = szTemp.AllocSysString();
		saRet.PutElement(index, bstr);
		SysFreeString(bstr);
		//}
	}
	CString strlefttable[36];
	strlefttable[0] = _T("Piston"); strlefttable[1] = _T("Focus"); strlefttable[2] = _T("Y tilt"); strlefttable[3] = _T("X tilt"); strlefttable[4] = _T("Sphereical 3rd order"); strlefttable[5] = _T("Sphereical 5rd order");
	strlefttable[6] = _T("Sphereical 7rd order"); strlefttable[7] = _T("Sphereical 9rd order"); strlefttable[8] = _T("Y coma 3rd order"); strlefttable[9] = _T("Y coma 5rd order"); strlefttable[10] = _T("Y coma 7rd order"); strlefttable[11] = _T("Y coma 9rd order");
	strlefttable[12] = _T("X coma 3rd order"); strlefttable[13] = _T("X coma 5rd order"); strlefttable[14] = _T("X coma 7rd order"); strlefttable[15] = _T("X coma 9rd order"); strlefttable[16] = _T("45 degree Asitig 3rd order"); strlefttable[17] = _T("45 degree Asitig 5rd order");
	strlefttable[18] = _T("45 degree Asitig 7rd order"); strlefttable[19] = _T("45 degree Asitig 9rd order"); strlefttable[20] = _T("0 degree Asitig 3rd order"); strlefttable[21] = _T("0 degree Asitig 5rd order"); strlefttable[22] = _T("0 degree Asitig 7rd order"); strlefttable[23] = _T("0 degree Asitig 9rd order");
	strlefttable[24] = _T("30 degree Tri 5th order"); strlefttable[25] = _T("30 degree Tri 7th order"); strlefttable[26] = _T("30 degree Tri 9th order"); strlefttable[27] = _T("0 degree Tri 5th order"); strlefttable[28] = _T("0 degree Tri 7th order"); strlefttable[29] = _T("0 degree Tri 9th order");
	strlefttable[30] = _T("22.5 degree Quad 7th order"); strlefttable[31] = _T("22.5 degree Quad 9th order"); strlefttable[32] = _T("0 degree Quad 7th order"); strlefttable[33] = _T("0 degree Quad 9th order"); strlefttable[34] = _T("18 degree Penta 9th order"); strlefttable[35] = _T("0 degree Penta 9th order");

	iCol = 3;
	for (int iRow = 1; iRow <= m_rows; iRow++)//将列表内容写入EXCEL
	{
		/*for (int iCol = 1; iCol <= m_cols; iCol++)
		{*/
		index[0] = iRow - 1;
		index[1] = iCol - 1;
		CString   szTemp=strlefttable[iRow-1];
		//rowstr.Format("%d", iRow);
		//szTemp.Format(_T("%5.2f"), m_zernCoef[iRow - 1]);
		BSTR   bstr = szTemp.AllocSysString();
		saRet.PutElement(index, bstr);
		SysFreeString(bstr);
		//}
	}
	range.put_ColumnWidth(_variant_t((long)30));
	iCol = 4;
	for (int iRow = 1; iRow <= m_rows; iRow++)//将列表内容写入EXCEL
	{
		/*for (int iCol = 1; iCol <= m_cols; iCol++)
		{*/
		index[0] = iRow - 1;
		index[1] = iCol - 1;
		CString   szTemp ;
		//rowstr.Format("%d", iRow);
		szTemp.Format(_T("%5.2f"), Sv[iRow - 1]);
		BSTR   bstr = szTemp.AllocSysString();
		saRet.PutElement(index, bstr);
		SysFreeString(bstr);
		//}
	}
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();
	book.SaveCopyAs(COleVariant(strFile));
	//       cellinterior.ReleaseDispatch();
	book.put_Saved(true);
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app.Quit();
	app.ReleaseDispatch();
}
void   CinterferomeryDlg::GetCellName(int nRow, int nCol, CString &strName)
{
	int nSeed = nCol;
	CString strRow;
	char cCell = 'A' + nCol - 1;
	strName.Format(_T("%c"), cCell);
	strRow.Format(_T("%d "), nRow);
	strName += strRow;
}

void CinterferomeryDlg::On32838()
{
	// TODO: 在此添加命令处理程序代码
	//创建sheet1并获取相关工作表的指针
	//BasicExcel xls;
	//xls.New(1);
	//BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
	//XLSFormatManager fmt_mgr(xls);
	////设置excel表格中的字体
	//ExcelFont font_bold;
	//font_bold._weight = FW_BOLD; //黑体
	//CellFormat fmt_bold(fmt_mgr);
	//fmt_bold.set_font(font_bold);
	//int col = 0, row = 0;
	////在第一列写入数据名称，也就是表头
	//for (row = 0; row < 36; row++)
	//{
	//	BasicExcelCell* cell = sheet->Cell(row, col);
	//	CString rowstr;
	//	rowstr.Format("%d", row);
	//	CString zerncofname = _T("zerncoff[") + rowstr + "]";
	//	cell->Set(zerncofname);
	//	cell->SetFormat(fmt_bold);
	//}
	//在第二列写下数据
	/*col = 1;
	for (row = 0; row < myData.size(); row++)
	{
		sheet->Cell(row, col)->Set(myData[row]);
	}*/
	//xls.SaveAs(filePath);
}


HBRUSH CinterferomeryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//// TODO:  在此更改 DC 的任何特性

	//// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	//return hbr;
	if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		pDC->SetBkMode(TRANSPARENT);
		//此处设置字体的颜色   
		pDC->SetTextColor(RGB(255, 255, 255));
		return   m_hbrush;
	}
	//if (pWnd->GetDlgCtrlID() == ID_START)
	//{
	//	//pDC->SetBkMode(TRANSPARENT);
	//	//此处设置字体的颜色   
	//	pDC->SetTextColor(RGB(255, 255, 255));
	//	return   m_hbrush;
	//}
}


void CinterferomeryDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
	{
		ReSize();
	}

	// TODO: 在此处添加消息处理程序代码
}
void CinterferomeryDlg::ReSize()
{
	float fsp[2];
	POINT Newp; //获取现在对话框的大小
	CRect recta;
	GetClientRect(&recta);     //取客户区大小  
	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;
	fsp[0] = (float)Newp.x / old.x;
	fsp[1] = (float)Newp.y / old.y;
	CRect Rect;
	int woc;
	CPoint OldTLPoint, TLPoint; //左上角
	CPoint OldBRPoint, BRPoint; //右下角
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件  
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//取得ID
		GetDlgItem(woc)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		OldTLPoint = Rect.TopLeft();
		TLPoint.x = long(OldTLPoint.x*fsp[0]);
		TLPoint.y = long(OldTLPoint.y*fsp[1]);
		OldBRPoint = Rect.BottomRight();
		BRPoint.x = long(OldBRPoint.x *fsp[0]);
		BRPoint.y = long(OldBRPoint.y *fsp[1]);
		Rect.SetRect(TLPoint, BRPoint);
		GetDlgItem(woc)->MoveWindow(Rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
		if (woc==1001)
		{
			showimgsizex = (BRPoint.x - TLPoint.x);
			showimgsizey = (BRPoint.y - TLPoint.y);
		}
	}
	old = Newp;

}

void CinterferomeryDlg::OnAperset()
{
	// TODO: 在此添加命令处理程序代码
	AperSet dlg;
	dlg.DoModal();
}
void CinterferomeryDlg::find_beiji_Region(Mat& Src, int AreaLimit, int CheckMode, int NeihborMode, int &beiji_count, vector<vector<Point2i>> &beiji_blob)
{
	int RemoveCount = 0;       //记录除去的个数  
							   //记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查  
	Mat Pointlabel = Mat::zeros(Src.size(), CV_8UC1);

	if (CheckMode == 1)
	{
		//cout << "Mode: 去除小区域. ";
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] < 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}
	else
	{
		//cout << "Mode: 去除孔洞. ";
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] > 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}

	vector<Point2i> NeihborPos;  //记录邻域点位置  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		//cout << "Neighbor mode: 8邻域." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	//else cout << "Neighbor mode: 4邻域." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//开始检测  
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 0)
			{
				//********开始该点处的检查**********  
				vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点  
				GrowBuffer.push_back(Point2i(j, i));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出  

				for (int z = 0; z < GrowBuffer.size(); z++)
				{

					for (int q = 0; q < NeihborCount; q++)                                      //检查四个邻域点  
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX < Src.cols&&CurrY >= 0 && CurrY < Src.rows)  //防止越界  
						{
							if (Pointlabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer  
								Pointlabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查  
							}
						}
					}

				}
				if (GrowBuffer.size() > AreaLimit &&GrowBuffer.size() < 70000)
				{
					vector<Point> points;
					for (int i = 0; i < GrowBuffer.size(); i++)
					{
						Point point;
						point.x = GrowBuffer[i].x;
						point.y = GrowBuffer[i].y;
						points.push_back(point);
					}
					RotatedRect bouding = minAreaRect(Mat(points));
					if (bouding.size.height > 50 || bouding.size.width > 50)
					{
						beiji_blob.push_back(GrowBuffer);
						beiji_count++;
					}

					/*				beiji_blob.push_back(GrowBuffer);
					beiji_count++;*/
				}

			}
		}
	}

}


bool CinterferomeryDlg::drawfringe(Mat &srcImage, vector<vector<Point2i>> beiji_blob, int CountNumer, Point &Currentcenter, int &Currentradius, int &radius, Point &center, Point StageCenterPoint, int&flag)
{
	if (beiji_blob.size() == 0)
	{
		return false;
	}
	for (int i = 0; i < beiji_blob.size(); i++)
	{
		vector<Point2i> tmpPoint;
		tmpPoint = beiji_blob[i];
		for (int j = 0; j < tmpPoint.size(); j++)
		{
			Point dots;
			dots.x = tmpPoint[j].x;
			dots.y = tmpPoint[j].y;
			circle(srcImage, dots, 0, 255, 1, 4);
		}
	}
	//imwrite("F:/TmpMat1.bmp", srcImage);

	//利用凸包检测+外接最小矩形-》得到最大内接圆
	vector<vector<Point2i> > allContour;
	vector<Point2i> Contour;
	vector<Point> points;//点值
	findContours(srcImage, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < allContour.size(); i++)
	{
		Contour = allContour[i];
		for (int j = 0; j < Contour.size(); j++)
		{
			Point point;
			point.x = Contour[j].x;
			point.y = Contour[j].y;
			points.push_back(point);
		}
	}

	vector<int> hull;
	convexHull(Mat(points), hull, false, true);
	srcImage.setTo(0);
	int hullcount = hull.size();//凸包的边数
	if (hullcount == 0) return false;
	if (hullcount > 0)
	{
		Point point0;
		point0 = points[(hull[hullcount - 1])];//连接凸包边的坐标点
		for (int i = 0; i < hullcount; i++)
		{
			Point point1;
			point1 = points[(hull[i])];
			line(srcImage, point0, point1, 255, 1, 8);   //将凸包点连接起来形成内边缘的外圈轨迹
			point0 = point1;
		}
	}

	//imwrite("F:/TmpMat2.bmp", srcImage);

	allContour.clear();
	Contour.clear();
	points.clear();
	findContours(srcImage, allContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (allContour.size() != 1) return false;
	Contour = allContour[0];
	for (int j = 0; j < Contour.size(); j++)
	{
		Point point;
		point.x = Contour[j].x;
		point.y = Contour[j].y;
		points.push_back(point);
	}
	//[5]寻找包围凸多边形的最小矩形
	RotatedRect bouding = minAreaRect(Mat(points));
	Currentcenter = bouding.center;
	Currentradius = min(bouding.size.width, bouding.size.height) / 2;

	if ((abs(bouding.center.x - StageCenterPoint.x) + abs(bouding.center.y - StageCenterPoint.y)) > 25 && CountNumer > 20)
	{
		flag = 1;
		return true;
	}

	/**************************************************************************************/


	if (Currentradius > radius && (abs(bouding.center.x - StageCenterPoint.x) + abs(bouding.center.y - StageCenterPoint.y)) < 25)
	{
		center = bouding.center;
		radius = min(bouding.size.width, bouding.size.height) / 2;
		return true;
	}
	/**************************************************************************************/
	return true;
}

void CinterferomeryDlg::On32843()
{
	// TODO: 在此添加命令处理程序代码
	TaskDispose::backgroundCircle = 0;
	TaskDispose::statechange = 1;
}


void CinterferomeryDlg::On32844()
{
	// TODO: 在此添加命令处理程序代码
	TaskDispose::backgroundCircle = 1;
	TaskDispose::statechange = 1;
}
