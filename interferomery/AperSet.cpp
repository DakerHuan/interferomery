// AperSet.cpp : 实现文件
//

#include "stdafx.h"
#include "interferomery.h"
#include "AperSet.h"
#include "afxdialogex.h"
HANDLE hMutex;

// AperSet 对话框
Rect g_rectangle;
Rect g_rectanglein;//环形区域内圆
bool AnnOutCirHaveDraw;//环形区域外圆是否绘制，1已经绘制，0未绘制
bool g_bDrawingBox = false;//是否进行绘制  
bool b_SelectRect = false;
Point oldMousePos;
static std::vector<std::vector<cv::Point>> vctvctPoint;
Point curPosPoint = Point(-1, -1);
static std::vector<cv::Point> vctPoint;

IMPLEMENT_DYNAMIC(AperSet, CDialogEx)
#define WM_UPDATEDATA  10000+1  // 更新控件值
#define WM_UPDATEVIEW  10000+2  //更新界面孔径大小

void DrawRectangle(cv::Mat& img, cv::Rect box)
{
	//cv::rectangle(img, box.tl(), box.br(), cv::Scalar(g_rng.uniform(0, 255), g_rng.uniform(0, 255), g_rng.uniform(0, 255)));//随机颜色  
	Mat clone;
	clone = img.clone();
	cv::rectangle(clone, box.tl(), box.br(), cv::Scalar(100, 200, 200), -1);
	cv::rectangle(img, box.tl(), box.br(), cv::Scalar(0, 0, 255), 2);
	double alpha;
	alpha = 0.8;
	addWeighted(img, alpha, clone, 1 - alpha, 0.0, img);
	Point center;
	center.x = min(box.tl().x, box.br().x) + abs(box.width / 2);
	center.y = min(box.tl().y, box.br().y) + abs(box.height / 2);
	TaskDispose::x = center.x;
	TaskDispose::y = center.y;
	if (box.width<0)
	{
		box.width *= -1;
	}
	if (box.height<0)
	{
		box.height *= -1;
	}
	TaskDispose::width = box.width;
	TaskDispose::height = box.height;
}
void DrawCircle(cv::Mat& img, cv::Rect box)
{
	Point center;
	int radius;
	radius = min(abs(box.br().x - box.tl().x), abs(box.br().y - box.tl().y)) / 2;
	center.x = min(box.tl().x, box.br().x) + radius;
	center.y = min(box.tl().y, box.br().y) + radius;
	TaskDispose::x = center.x;
	TaskDispose::y = center.y;
	TaskDispose::radius = radius;
	if (radius > 2)
	{
		Mat clone;
		img.copyTo(clone);
		cv::circle(clone, center, radius, cv::Scalar(100, 200, 200), -1);
		cv::circle(img, center, radius, cv::Scalar(0, 0, 255), 2);
		double alpha;
		alpha = 0.8;
		//clone.copyTo(img);
		addWeighted(img, alpha, clone, 1 - alpha, 0.0, img);
		if (b_SelectRect)
		{
			cv::rectangle(img, Point(center.x-radius,center.y-radius),Point(center.x+radius,center.y+radius), cv::Scalar(255, 255, 255), 1);
		}
	}
}
void DrawInCircle(cv::Mat& img, cv::Mat& srcimg, cv::Rect box)
{
	Mat outcircle;
	img.copyTo(outcircle);
	outcircle.setTo(0);
	Point centerout;
	int radiusout;
	radiusout = min(abs(g_rectangle.br().x - g_rectangle.tl().x), abs(g_rectangle.br().y - g_rectangle.tl().y)) / 2;
	centerout.x = min(g_rectangle.tl().x, g_rectangle.br().x) + radiusout;
	centerout.y = min(g_rectangle.tl().y, g_rectangle.br().y) + radiusout;
	DrawCircle(outcircle, g_rectangle);
	
	Point center;
	int radius;
	radius = min(abs(box.br().x - box.tl().x), abs(box.br().y - box.tl().y)) / 2;
	center.x = min(box.tl().x, box.br().x) + radius;
	center.y = min(box.tl().y, box.br().y) + radius;
	TaskDispose::rin = radius;
	//if (radius > 2)
	//{
		Mat incircle;
		img.copyTo(incircle);
		incircle.setTo(0);
		cv::circle(incircle, center, radius, cv::Scalar(100, 200, 200), -1);
		//cv::circle(incircle, center, radius, cv::Scalar(0, 0, 255),2);
		Mat annula;
		img.copyTo(annula);
		annula.setTo(0);
		addWeighted(outcircle, 1, incircle, -1, 0.0, annula);

		//cv::circle(img, center, radius, cv::Scalar(0, 0, 255), 2);
		double alpha;
		alpha = 0.5;
		//clone.copyTo(img);
		//addWeighted(srcimg, 0.5, annula,0.5, 0.0, annula);
		addWeighted(srcimg, 1, annula, 0.5, 0.0, srcimg);
		cv::circle(srcimg, centerout, radiusout, cv::Scalar(0, 0, 255), 2);
		cv::circle(srcimg, center, radius, cv::Scalar(0, 150, 255), 2);
		//annula.copyTo(srcimg);
	//}
}
void DrawEllipse(cv::Mat& img, cv::Rect box)
{
	Point center;
	center.x = min(box.tl().x, box.br().x) + abs(box.width / 2);
	center.y = min(box.tl().y, box.br().y) + abs(box.height / 2);
	TaskDispose::x = center.x;
	TaskDispose::y = center.y;
	if (box.width < 0)
	{
		box.width *= -1;
	}
	if (box.height < 0)
	{
		box.height *= -1;
	}
	TaskDispose::width = box.width;
	TaskDispose::height = box.height;
	Mat clone;
	clone = img.clone();
	cv::ellipse(clone, center, Size(abs(box.width / 2), abs(box.height / 2)), 0, 0, 360, cv::Scalar(100, 200, 200), -1);
	cv::ellipse(img, center, Size(abs(box.width / 2), abs(box.height / 2)), 0, 0, 360, cv::Scalar(0, 0, 255), 2);
	double alpha;
	alpha = 0.8;
	addWeighted(img, alpha, clone, 1 - alpha, 0.0, img);
	if (b_SelectRect)
	{
		cv::rectangle(img, Point(center.x - TaskDispose::width/2, center.y - TaskDispose::height/2), Point(center.x + TaskDispose::width / 2, center.y + TaskDispose::height/2), cv::Scalar(255, 255, 255), 1);
	}
}
LRESULT AperSet::OnUpdateView(WPARAM wParam, LPARAM lParam)
{
	//UpdateData(true);
	//TaskDispose::aperType = m_iRadio1;
	if (TaskDispose::radius < 10)
	{
		TaskDispose::radius = 10;
	}
	m_x = GetDlgItemInt(IDC_X);
	if (m_x<2)
	{
		m_x = 2;
	}
	m_y = GetDlgItemInt(IDC_Y);
	if (m_y<2)
	{
		m_y = 2;
	}
	TaskDispose::x = m_x;
	TaskDispose::y = m_y;
	m_radius = GetDlgItemInt(IDC_RADIUS);
	if (TaskDispose::aperType == 0)
	{
		if (m_radius<2)
		{
			m_radius = 2;
		}

		TaskDispose::radius=m_radius;
		g_rectangle.x = TaskDispose::x - TaskDispose::radius;
		g_rectangle.y = TaskDispose::y - TaskDispose::radius;
		g_rectangle.width = TaskDispose::radius * 2;
		g_rectangle.height = TaskDispose::radius * 2;
	}
	if (TaskDispose::aperType == 1)
	{
		m_height = GetDlgItemInt(IDC_HEIGHT);
		m_width = GetDlgItemInt(IDC_WIDTH);
		TaskDispose::height=m_height;
		TaskDispose::width=m_width;
		g_rectangle.x = TaskDispose::x - TaskDispose::width/2;
		g_rectangle.y = TaskDispose::y - TaskDispose::height/2;
		g_rectangle.width = TaskDispose::width;
		g_rectangle.height = TaskDispose::height;
	}
	else if (TaskDispose::aperType == 2)
	{
		m_heightellipse = GetDlgItemInt(IDC_HEIGHTELLIPSE);
		m_widthellipse = GetDlgItemInt(IDC_WIDTHELLIPSE);
		TaskDispose::height=m_heightellipse;
		TaskDispose::width=m_widthellipse;
		g_rectangle.x = TaskDispose::x - TaskDispose::width / 2;
		g_rectangle.y = TaskDispose::y - TaskDispose::height / 2;
		g_rectangle.width = TaskDispose::width;
		g_rectangle.height = TaskDispose::height;
	}
	 
	if (TaskDispose::aperType == 4)
	{
		m_rin = GetDlgItemInt(IDC_RIN);
		m_rout = GetDlgItemInt(IDC_ROUT);
		if (m_rout-m_rin<2)
		{
			m_rin = m_rout - 2;
		}
		TaskDispose::rin=m_rin;
		TaskDispose::radius=m_rout;
		g_rectangle.x = TaskDispose::x - TaskDispose::radius;
		g_rectangle.y = TaskDispose::y - TaskDispose::radius;
		g_rectangle.width = TaskDispose::radius * 2;
		g_rectangle.height = TaskDispose::radius * 2;
		g_rectanglein.x = TaskDispose::x - TaskDispose::rin;
		g_rectanglein.y = TaskDispose::y - TaskDispose::rin;
		g_rectanglein.width = TaskDispose::rin * 2;
		g_rectanglein.height = TaskDispose::rin * 2;
	}
	return 0;
}
LRESULT AperSet::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	//UpdateData(wParam);
	if (TaskDispose::radius < 10)
	{
		TaskDispose::radius = 10;
	}
	if (TaskDispose::aperType==0)
	{
		m_radius = TaskDispose::radius;
	}
	if (TaskDispose::aperType == 1)
	{
		m_height = TaskDispose::height;
		m_width = TaskDispose::width;
	}
	else if (TaskDispose::aperType == 2)
	{
		m_heightellipse = TaskDispose::height;
		m_widthellipse = TaskDispose::width;
	}
	m_x = TaskDispose::x;
	m_y = TaskDispose::y;
	if (TaskDispose::aperType==4)
	{
		m_rin = TaskDispose::rin;
		m_rout = TaskDispose::radius;
	}
	
	
	UpdateData(FALSE);
	return 0;
}
void   on_mouse(int event, int x, int y, int flags, void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号    
{
	TaskDispose::mouseacitive = true;
	TaskDispose::LButDownAndMove = false;
	if (TaskDispose::aperType==3)
	{
		curPosPoint = Point(-1, -1);
		Mat& image = *(cv::Mat*) ustc;
		static cv::Point cur_pt = Point(-1, -1);
		if (event == CV_EVENT_LBUTTONDBLCLK)
		{
			if (vctPoint.size() > 3)
			{
				TaskDispose::choosedone = true;
			}
		}
		if (event == CV_EVENT_LBUTTONUP)
		{
			cur_pt = cv::Point(x, y);
			if (!TaskDispose::choosedone)
			{
				vctPoint.push_back(cur_pt);
			}
		}
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
		{
			cur_pt = cv::Point(x, y);
			curPosPoint = cur_pt;
		}
		else if (event == CV_EVENT_RBUTTONUP)
		{
			TaskDispose::choosedone = false;
			vctPoint.clear();
		}
		return;
	}
	//Mat& image = *(cv::Mat*) ustc;
	switch (event)
	{
		//左键按下消息  
	case EVENT_LBUTTONDBLCLK:
		{
			if (TaskDispose::aperType==0)
			{
				if (sqrt((x - TaskDispose::x)*(x - TaskDispose::x) + (y - TaskDispose::y)*(y - TaskDispose::y)) < TaskDispose::radius)
				{ 
					b_SelectRect = !b_SelectRect;
			     }	
			}
			if (TaskDispose::aperType==2&&TaskDispose::width!=0&TaskDispose::height!=0)
			{
				if ( ( (float)((x-TaskDispose::x)*(x-TaskDispose::x)*4/(TaskDispose::width*TaskDispose::width))+ (float)((y - TaskDispose::y)*(y - TaskDispose::y)*4 / (TaskDispose::height*TaskDispose::height)))<1)
				{
					b_SelectRect = !b_SelectRect;
				}
			}
		}
		break;
	case EVENT_LBUTTONDOWN:
	{
		if (b_SelectRect&&(TaskDispose::aperType == 0 || TaskDispose::aperType == 2))
		{
			oldMousePos.x = x;
			oldMousePos.y = y;
			g_bDrawingBox = true;
		}
		else
		{
			g_bDrawingBox = true;
			if (!AnnOutCirHaveDraw)
			{
				g_rectangle = Rect(x, y, 0, 0);//记录起始点  
			}
		}
	}
	break;
		//鼠标移动消息  
	case EVENT_MOUSEMOVE:
	{
		if (g_bDrawingBox&&!b_SelectRect)//如果是否进行绘制的标识符为真，则记录下长和宽到RECT型变量中  
		{
			TaskDispose::LButDownAndMove = true;
			if (!AnnOutCirHaveDraw)
			{
				g_rectangle.width = x - g_rectangle.x;
				g_rectangle.height = y - g_rectangle.y;
			}
			else
			{
				int radiusout = min(g_rectangle.width, g_rectangle.height) / 2;
				int centerx = g_rectangle.x + radiusout;
				int centery = g_rectangle.y + radiusout;

				g_rectanglein.width = abs(x - centerx) * 2;
				g_rectanglein.height = abs(y - centery) * 2;
				int radiusin = min(g_rectanglein.width, g_rectanglein.height) / 2;
				if (radiusout - radiusin<5)
				{
					radiusin = radiusout - 5;
					g_rectanglein.width = g_rectanglein.height = radiusin * 2;
				}
				g_rectanglein.x = centerx - radiusin;
				g_rectanglein.y = centery - radiusin;

			}
		}
		if (b_SelectRect&&g_bDrawingBox)
		{
			if (TaskDispose::aperType==0)
			{
				if (x>g_rectangle.x&&y>g_rectangle.y&&(x<(g_rectangle.x+2*TaskDispose::radius))&&(y<(g_rectangle.y+2*TaskDispose::y)))
				{
					TaskDispose::LButDownAndMove = true;
					if (sqrt((x - TaskDispose::x)*(x - TaskDispose::x) + (y - TaskDispose::y)*(y - TaskDispose::y)) < TaskDispose::radius)
					{
						g_rectangle.x += x - oldMousePos.x;
						g_rectangle.y += y - oldMousePos.y;
					}
					else
					{
						int oldradius = sqrt((oldMousePos.x - TaskDispose::x)*(oldMousePos.x - TaskDispose::x) + (oldMousePos.y - TaskDispose::y)*(oldMousePos.y - TaskDispose::y)) - TaskDispose::radius;
						int newradius = sqrt((x - TaskDispose::x)*(x - TaskDispose::x) + (y - TaskDispose::y)*(y - TaskDispose::y)) - TaskDispose::radius;
						TaskDispose::radius += newradius-oldradius;
						g_rectangle.x = TaskDispose::x - TaskDispose::radius;
						g_rectangle.y = TaskDispose::y - TaskDispose::radius;
						g_rectangle.width = TaskDispose::radius * 2;
						g_rectangle.height = TaskDispose::radius * 2;
					}
				}
			}
			if (TaskDispose::aperType==2)
			{
				if (TaskDispose::width!=0&&TaskDispose::height!=0&&x > g_rectangle.x&&y > g_rectangle.y && (x < (g_rectangle.x + TaskDispose::width)) && (y < (g_rectangle.y + TaskDispose::height)))
				{
					TaskDispose::LButDownAndMove = true;
					if (((float)((x - TaskDispose::x)*(x - TaskDispose::x)*4 /(float) (TaskDispose::width*TaskDispose::width)) + (float)((y - TaskDispose::y)*(y - TaskDispose::y)*4 /(float) (TaskDispose::height*TaskDispose::height))) < 1)
					{
						g_rectangle.x += x - oldMousePos.x;
						g_rectangle.y += y - oldMousePos.y;
					}
					else
					{
						int centlengthx = abs(x - TaskDispose::x);
						int oldcentlengthx = abs(oldMousePos.x - TaskDispose::x);
						int centlengthy = abs(y - TaskDispose::y);
						int oldcentlengthy = abs(oldMousePos.y - TaskDispose::y);
						int movex = centlengthx - oldcentlengthx;
						int movey = centlengthy - oldcentlengthy;
						g_rectangle.x -= movex;
						g_rectangle.y -= movey;
						g_rectangle.width += movex * 2;
						g_rectangle.height += movey * 2;
						TaskDispose::width = g_rectangle.width;
						TaskDispose::height = g_rectangle.height;
					}

				}
			}
			oldMousePos.x = x;
			oldMousePos.y = y;
		}

	}

	break;

	
	case  EVENT_RBUTTONUP:
	{
		AnnOutCirHaveDraw = false;
		g_rectangle = Rect(-1, -1, 0, 0);
		g_rectanglein = Rect(-1, -1, 0, 0);
		TaskDispose::LButDownAndMove = false;
	}
	break;
	//左键抬起消息  
	case EVENT_LBUTTONUP:
	{
		TaskDispose::LButDownAndMove = false;
		g_bDrawingBox = false;//置标识符为false  						  
		if (!AnnOutCirHaveDraw)
		{
			if (g_rectangle.width < 0)
			{
				g_rectangle.x += g_rectangle.width;
				g_rectangle.width *= -1;
			}

			if (g_rectangle.height < 0)
			{
				g_rectangle.y += g_rectangle.height;
				g_rectangle.height *= -1;
			}
		}
		else if (TaskDispose::aperType == 4)
		{
			AnnOutCirHaveDraw = true;
		}
	}
	break;

	}
}
HANDLE HThread;
UINT MyThreadFunction(LPVOID lpParameter)
{
	AperSet *pDlg = (AperSet*)lpParameter;
	
	bool stopAperWnd=false;
	char temp[16];
	g_rectangle = Rect(-1, -1, 0, 0);
	g_rectanglein = Rect(-1, -1, 0, 0);
	//TaskDispose::choosedone = false;
	string windowname = "myshowWnd";
	//namedWindow(windowname);
	//cv::VideoCapture vcap("G:/科研项目/干涉图像处理/测试视频/单件.avi");
	//cv::VideoCapture vcap1(0);
	//vcap1.set(CV_CAP_PROP_FPS, 20);
	Mat srcImage(720, 540, CV_8UC3, cv::Scalar::all(0));
	Mat tempImage(720,540, CV_8UC3, cv::Scalar::all(0));
	setMouseCallback("myshowWnd", on_mouse, (void*)&srcImage);
	Mat readframe(640, 480, CV_8UC3, cv::Scalar::all(0));
	//Mat readframe;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	while (!stopAperWnd)
	{
		WaitForSingleObject(hMutex, INFINITE);
		TaskDispose::mouseacitive = false;
		cv::waitKey(50);
		if (!cvGetWindowHandle("myshowWnd"))
		{
			cv::destroyWindow(windowname);
			stopAperWnd = true;
			break;
		}

		/*if ((!vcap1.read(readframe)))
		{
			waitKey(200);
			continue;
		}*/
		//vcap >> readframe;
		//resize(readframe, srcImage, Size(720, 540), 0, 0, INTER_LINEAR);
		//srcImage = TaskDispose::TaskSrcFrame;
		TaskDispose::TaskSrcFrame.copyTo(srcImage);
		//int dep = readframe.depth();
		//int cha = readframe.channels();
		srcImage.copyTo(tempImage);//拷贝源图到临时变量  
		
		//if (g_bDrawingBox || TaskDispose::choosedone) 
			switch (TaskDispose::aperType)
			{
			case 0:
				DrawCircle(tempImage, g_rectangle);
				break;
			case 1:
				DrawRectangle(tempImage, g_rectangle);
				break;
			case 2:
				DrawEllipse(tempImage, g_rectangle);
				break;
			}
			if (TaskDispose::aperType==3)
			{
				if (!TaskDispose::choosedone)
				{
					if (vctPoint.size() == 1)
					{
						circle(tempImage, vctPoint.at(0), 2, cv::Scalar(0, 255, 0), 5, 8, 0);
						sprintf_s(temp, "%d", vctPoint.size() - 1);
						putText(tempImage, temp, vctPoint.at(vctPoint.size() - 1), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0, 0));
					}
					if (vctPoint.size() > 1)
					{
						for (int i = 0; i < vctPoint.size() - 1; i++)
						{
							sprintf_s(temp, "%d", i);
							putText(tempImage, temp, vctPoint.at(i), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0, 0));
							cv::line(tempImage, vctPoint.at(i), vctPoint.at(i + 1), cv::Scalar(0, 255, 0), 5, 8, 0);
						}
						sprintf_s(temp, "%d", vctPoint.size() - 1);
						putText(tempImage, temp, vctPoint.at(vctPoint.size() - 1), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0, 0));
						if (curPosPoint != Point(-1, -1))
						{
							cv::line(tempImage, vctPoint.at(vctPoint.size() - 1), curPosPoint, cv::Scalar(0, 255, 0), 5, 8, 0);
						}
					}
				}
				else
				{
					int vctsize = vctPoint.size();
					int lineType = 8;
					int w = 400;
					Point rook_points[1][50];
					for (int i = 0; i < vctsize; i++)
					{
						rook_points[0][i] = vctPoint.at(i);
					}

					const Point* ppt2[1] = { rook_points[0] };
					int npt[] = { vctsize };
					Mat clone;
					clone = tempImage.clone();
					double alpha;
					alpha = 0.8;
					polylines(tempImage, ppt2, npt, 1, 1, Scalar(0, 0, 255, 0), 2, lineType, 0);
					fillPoly(clone, ppt2, npt, 1, Scalar(100, 200, 200, 0), lineType);
					addWeighted(tempImage, alpha, clone, 1 - alpha, 0.0, tempImage);
				}
			}
			if (TaskDispose::aperType==4)
			{
				if (g_bDrawingBox || TaskDispose::choosedone || AnnOutCirHaveDraw)
				{
					DrawCircle(tempImage, g_rectangle);//当进行绘制的标识符为真，则进行绘制  
					if (AnnOutCirHaveDraw)
					{
						DrawInCircle(tempImage,srcImage,g_rectanglein);//当进行绘制的标识符为真，则进行绘制  
						//srcImage.copyTo(tempImage);
						tempImage = srcImage.clone();
					}
				}
			}
		rectangle(tempImage, TaskDispose::origin, Point(TaskDispose::origin.x + 512, TaskDispose::origin.y + 512), cv::Scalar(100, 200, 200), 3);
		if (!cvGetWindowHandle("myshowWnd"))
		{
			cv::destroyWindow(windowname);
			stopAperWnd = true;
			break;
		}
		imshow("myshowWnd", tempImage);
		//imshow("testwnd", tempImage);
		if (waitKey(10) == 27)
		{
			destroyWindow("myshowWnd");
			break;//按下ESC键，程序退出  
		}
		if (TaskDispose::LButDownAndMove)
		{
			SendMessage(pDlg->m_hWnd, WM_UPDATEDATA, FALSE, FALSE);
		}
		if(!TaskDispose::mouseacitive)
		{
			SendMessage(pDlg->m_hWnd, WM_UPDATEVIEW, FALSE, FALSE);
		}
		ReleaseMutex(hMutex);
	}
	//vcap1.release();
	return 0;
}

AperSet::AperSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_APERSET, pParent)
	, m_x(TaskDispose::x)
	, m_y(TaskDispose::y)
	, m_radius(TaskDispose::radius)
	, m_height(TaskDispose::height)
	, m_width(TaskDispose::width)
	, m_rout(TaskDispose::rout)
	, m_rin(TaskDispose::rin)
	, m_iRadio1(TaskDispose::aperType)
	, m_heightellipse(TaskDispose::height)
	, m_widthellipse(TaskDispose::width)
	, m_step(10)
{
	//HThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, (LPVOID)this, 0, 0);
	//HWND hWnd = (HWND)cvGetWindowHandle("view");
	//HWND hParent = ::GetParent(hWnd);
	//::SetParent(hWnd, GetDlgItem(IDC_STATIC)->m_hWnd);
	//::ShowWindow(hParent, SW_HIDE);
	
}

AperSet::~AperSet()
{
	//ExitThread(MyThreadFunction);
	//AfxEndThread(MyThreadFunction);
	//TerminateThread(HThread,0);
	destroyWindow("myshowWnd");
	
}

void AperSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_X, m_x);
	//DDV_MinMaxInt(pDX, m_x, 2, 510);
	DDX_Text(pDX, IDC_Y, m_y);
	//DDV_MinMaxInt(pDX, m_y, 2, 510);
	DDX_Text(pDX, IDC_RADIUS, m_radius);
	//DDV_MinMaxInt(pDX, m_radius, 2, 255);
	DDX_Text(pDX, IDC_HEIGHT, m_height);
	//DDV_MinMaxInt(pDX, m_height, 2, 510);
	DDX_Text(pDX, IDC_WIDTH, m_width);
	//DDV_MinMaxInt(pDX, m_width, 2, 510);
	DDX_Text(pDX, IDC_ROUT, m_rout);
	//DDV_MinMaxInt(pDX, m_rout, 5, 254);
	DDX_Text(pDX, IDC_RIN, m_rin);
	//DDV_MinMaxInt(pDX, m_rin, 2, 250);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadio1);
	DDX_Text(pDX, IDC_HEIGHTELLIPSE, m_heightellipse);
	DDX_Text(pDX, IDC_WIDTHELLIPSE, m_widthellipse);
	DDX_Text(pDX, IDC_STEP, m_step);
}


BEGIN_MESSAGE_MAP(AperSet, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &AperSet::OnBnClickedButton1)
	ON_MESSAGE(WM_UPDATEDATA, OnUpdateData)
	ON_MESSAGE(WM_UPDATEVIEW,OnUpdateView)
	ON_BN_CLICKED(IDC_RADIO1, &AperSet::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &AperSet::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &AperSet::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &AperSet::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &AperSet::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_BUTTONUP, &AperSet::OnBnClickedButtonup)
	ON_BN_CLICKED(IDC_BUTTONDOWN, &AperSet::OnBnClickedButtondown)
	ON_BN_CLICKED(IDC_BUTTONLEFT, &AperSet::OnBnClickedButtonleft)
	ON_BN_CLICKED(IDC_BUTTONRIGHT, &AperSet::OnBnClickedButtonright)
END_MESSAGE_MAP()





void AperSet::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//stopAperWnd = true;
	//WaitForSingleObject(hMutex, INFINITE);
	UpdateData(true);
	TaskDispose::aperType = m_iRadio1;
	TaskDispose::x = m_x;
	TaskDispose::y = m_y;
	TaskDispose::radius = m_radius;
	if (TaskDispose::aperType ==1)
	{
		TaskDispose::width = m_width;
		TaskDispose::height = m_height;
	}
	else if (TaskDispose::aperType==2)
	{
		TaskDispose::width = m_widthellipse;
		TaskDispose::height = m_heightellipse;
	}
	TaskDispose::rin = m_rin;
	TaskDispose::rout = m_rout;
	destroyWindow("myshowWnd");
	//TerminateThread(HThread, 0);
	//WaitForSingleObject(HThread, INFINITE);
	
	CreateEffectPtMap();
	//vctPointglobal = vctPoint;
	EndDialog(IDOK);
	//ReleaseMutex(hMutex);
}

void AperSet::CreateEffectPtMap()
{
	int m_aperType = TaskDispose::aperType;
	Point center;
	center.x = TaskDispose::x-TaskDispose::origin.x;
	center.y = TaskDispose::y-TaskDispose::origin.y;
	int m_aperRadius = TaskDispose::radius;
	int m_aperHeight = TaskDispose::height;
	int m_aperWidth = TaskDispose::width;
	int m_aperRin = TaskDispose::rin;
	int m_aperRout = TaskDispose::rout;
	Mat tempPtMap(512, 512, CV_8UC1, cv::Scalar::all(0));

	Rect rect;
	rect.width = m_aperWidth;
	rect.height = m_aperHeight;
	//if ((center.x - rect.width / 2) < 0 || (center.y - rect.height / 2) < 0 || (center.x + rect.width / 2) > 512 || (center.y + rect.height / 2) > 512)
	//{
	//	MessageBox(_T("Aper size is setted wrong! Please reset!"));
	//}
	rect.x = center.x - rect.width / 2;
	rect.y = center.y - rect.height / 2;
	//if ((center.x - m_aperRadius) < 0 || (center.y - m_aperRadius) < 0 || (center.x + m_aperRadius) > 512 || (center.y + m_aperRadius) > 512)
	//{
	//	MessageBox(_T("Aper size is setted wrong! Please reset!"));
	//}
	//if ((center.x - m_aperRout) < 0 || (center.y - m_aperRout) < 0 || (center.x + m_aperRout) > 512 || (center.y + m_aperRout) > 512)
	//{
	//	MessageBox(_T("Aper size is setted wrong! Please reset!"));
	//}
	switch (m_aperType)
	{
	case 0:
		circle(tempPtMap, center, m_aperRadius, Scalar(1), -1);
		break;
	case 1:
		tempPtMap(rect).setTo(1);
		break;
	case 2:
		cv::ellipse(tempPtMap, center, Size(abs(rect.width / 2), abs(rect.height / 2)), 0, 0, 360, Scalar(1), -1);
		break;
	case 3:
		if (TaskDispose::choosedone)
		{
			int vctsize = vctPoint.size();
			int lineType = 8;
			//int w = 400;
			Point rook_points[1][50];
			for (int i = 0; i < vctsize; i++)
			{
				rook_points[0][i] = vctPoint.at(i);
				rook_points[0][i].x -= TaskDispose::origin.x;
				rook_points[0][i].y -= TaskDispose::origin.y;
			}

			const Point* ppt2[1] = { rook_points[0] };
			int npt[] = { vctsize };
			fillPoly(tempPtMap, ppt2, npt, 1, Scalar(1), lineType);
		}
		break;
	case 4:
		circle(tempPtMap, center, m_aperRout, Scalar(1), -1);
		circle(tempPtMap, center, m_aperRin, Scalar(0), -1);
		break;
	}
	tempPtMap.copyTo(TaskDispose::PtMap);
	//imshow("myshow", tempPtMap*255);
}


BOOL AperSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	//((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);//选上
	//((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	//((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
	//CRect rect;
	//GetDlgItem(IDC_STATIC)->GetClientRect(rect);
	AnnOutCirHaveDraw = false;
	g_rectangle = Rect(-1, -1, 0, 0);
	g_rectanglein = Rect(-1, -1, 0, 0);

	cv::namedWindow("myshowWnd", WINDOW_AUTOSIZE);
	//cv::resizeWindow("myshowWnd",720, 576);
	HWND hWnd = (HWND)cvGetWindowHandle("myshowWnd");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_SHOWIMG)->m_hWnd);
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_SHOWIMG); //获取控件指针，IDC_BUTTON1为控件ID号
	//pWnd->SetWindowPos(NULL, 20, 20, 720, 576, SWP_NOZORDER );
	::ShowWindow(hParent, SW_HIDE);

	CRect rect;
	GetDlgItem(IDC_SHOWIMG)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(IDC_SHOWIMG)->MoveWindow(rect.left, rect.top, 720, 540, true);


	m_iRadio1 = TaskDispose::aperType;
	  m_x= TaskDispose::x;
	  m_y= TaskDispose::y;
	  m_radius= TaskDispose::radius;
	  m_width= TaskDispose::width;
	  m_height= TaskDispose::height;
	  m_rin= TaskDispose::rin;
	  m_rout= TaskDispose::rout;
	UpdateData(FALSE);
	
	HThread= CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MyThreadFunction, this, 0, 0);
	//HThread = AfxBeginThread(MyThreadFunction, this);
	//m_pthread=AfxBeginThread(MyThreadFunction, this, THREAD_PRIORITY_NORMAL, 0, 0);
	HBITMAP   hBitmap;
	hBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP2)); // IDB_BITMAP1为资源图片ID 
	((CButton *)GetDlgItem(IDC_BUTTONUP))->SetBitmap(hBitmap);
	hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3)); // IDB_BITMAP1为资源图片ID 
	((CButton *)GetDlgItem(IDC_BUTTONDOWN))->SetBitmap(hBitmap);
	hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP4)); // IDB_BITMAP1为资源图片ID 
	((CButton *)GetDlgItem(IDC_BUTTONLEFT))->SetBitmap(hBitmap);
	hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP5)); // IDB_BITMAP1为资源图片ID 
	((CButton *)GetDlgItem(IDC_BUTTONRIGHT))->SetBitmap(hBitmap);
	return TRUE;  // return TRUE unless you set the focus to a control
}






void AperSet::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	TaskDispose::aperType = m_iRadio1;
}


void AperSet::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	TaskDispose::aperType = m_iRadio1;
}


void AperSet::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	TaskDispose::aperType = m_iRadio1;
}


void AperSet::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	TaskDispose::aperType = m_iRadio1;
}


void AperSet::OnBnClickedRadio5()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	TaskDispose::aperType = m_iRadio1;
	//TaskDispose::choosedone = true;
	AnnOutCirHaveDraw = true;
}


void AperSet::OnBnClickedButtonup()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (TaskDispose::aperType == 3)
	{
		for (int i = 0; i < vctPoint.size(); i++)
		{
			vctPoint.at(i).y -= m_step;
		}
		return;
	}
	m_y -= m_step;
	UpdateData(FALSE);
}




void AperSet::OnBnClickedButtondown()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (TaskDispose::aperType==3)
	{
		for (int i = 0; i < vctPoint.size();i++)
		{
			vctPoint.at(i).y+=m_step;
		}
		return;
	}
	m_y += m_step;
	UpdateData(FALSE);
}


void AperSet::OnBnClickedButtonleft()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (TaskDispose::aperType == 3)
	{
		for (int i = 0; i < vctPoint.size(); i++)
		{
			vctPoint.at(i).x -= m_step;
		}
		return;
	}
	m_x -= m_step;
	UpdateData(FALSE);
}


void AperSet::OnBnClickedButtonright()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (TaskDispose::aperType == 3)
	{
		for (int i = 0; i < vctPoint.size(); i++)
		{
			vctPoint.at(i).x += m_step;
		}
		return;
	}
	m_x += m_step;
	UpdateData(FALSE);
}
