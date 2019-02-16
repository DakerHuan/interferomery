
// interferomeryDlg.h : ͷ�ļ�
//

#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/core/utility.hpp>
#include "opencv2/opencv.hpp"
//#include "opencv2/core.hpp"
#include "opencv2/core/core.hpp"
#include<iostream> 
#include "NewMenu.h"
using namespace cv;
using namespace std;
//#include <cv.h>  
//#include <cxcore.h>  
//#include <highgui.h> 
#include "CvvImage.h"
#include "AperSet.h"
#include "InfSet.h"
#include "TaskDispose.h"
#include "afxwin.h"
#include "afxvslistbox.h"
#include <windowsx.h>

#define PI		3.1415926  
#define LENGTH 512
#define WHITE 255
#define BLACK 0
//#define	NON		-2147483647 //-32767  // Non Effective Point Value
#define	NON		-2147483647 //-32767  // Non Effective Point Value
#define BLOCKNUMMAX	255 //������
#define MAXBLOCKNUM	20 //������ڿ���
#define REASONABLEBLOCKNUM	40	//��������С��Ŀ
#define	POSITIVE	1  //����Ϊ��
#define NEGATIVE	2  //����Ϊ��
#define UNDEFINED	0  //���Ż�����δ��
#define BRIGHT		2  //����Ϊ��
#define DARK		1  //����Ϊ��
#define	UNPROCESSED	100 // Used for Unwrapping 
#define	NUMBER	3		// Used for Define PV 
#define	PLANE	0       // Style flag for plane
#define	SPHERE	1       // Style flag for sphere
#define ORIGINAL	2	// Style flag for orignal wavefront
#define BAND1	0		// smaller than wave/10
#define BAND2	1		// between wave/10 and wave/2
#define BAND3	2		// greater than wave/2  
#define	NOFILTER	-1	// filter is not active

#define WM_STATICANAYSIS 10000+3 //��̬������Ϣ
typedef struct {
	unsigned char	bw; //��Ϊ�ڵĻ��ǰ׵�
	int				order; //����
	int				NeighbourBlock[MAXBLOCKNUM]; //���ڵĿ��
	int				NeighbourLength[MAXBLOCKNUM]; //���ڱ�Ե����
	int				NeighbourNum; //���ڵĿ���
	BOOL			bDefine; //�����ÿ�ļ����Ƿ�ȷ��
	BOOL			bDeleted; //�����ÿ��Ƿ��ѱ�ɾ��
	int				TotalPts; //�������ĵ���
	int				Px; //���м�ֵ�������
	int				Py;
} BLOCKINF;

class CinterferomeryDlg : public CDialogEx
{
// ����
public:
	CinterferomeryDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTERFEROMERY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//VideoCapture captue;//����һ������ͷ������ָ������ͷ��ţ�ֻ��һ��д0�Ϳ���
	void find_beiji_Region(Mat& Src, int AreaLimit, int CheckMode, int NeihborMode, int &beiji_count, vector<vector<Point2i>> &beiji_blob);
	bool drawfringe(Mat &srcImage, vector<vector<Point2i>> beiji_blob, int CountNumer, Point &Currentcenter, int &Currentradius, int &radius, Point &center, Point StageCenterPoint, int&flag);
	void DrawPicToHDC(IplImage *img, UINT ID);//��opecvͼ����Ƶ�MFC�Ի���
	Mat  CreateEffectPtMap();//������Ч����ͼ
	Mat m_PtMap;
	Mat m_BWMap;
	unsigned char(*Image)[LENGTH]; // interferogram
	int(*WavePhase)[LENGTH]; // Phase Data	 
	float	m_fringeNumber;	// fringe number of interfereogram
	int	m_Type;	// wavefront type  
	int	m_bScanWay;	//TRUE--data is obtained by phase shifting method
	int	m_measureBand;		// measure band
	Mat  BWinterferogram();//����ͼ��ֵ������
	unsigned char	m_number;	// number of seperated wavefront 
	unsigned char(*ucharPtMap)[LENGTH];
	unsigned char(*m_flag)[LENGTH]; // flag for wavefront
	void BWPreprocess(Mat& bwimg, Mat ptimg);//�Զ�ֵ��ͼ�����Ԥ����
	//void NoiseRemoveInterferogram(Mat& input, Mat& output);//�ԻҶ�ͼƵ���ͨ�˲�����
	void LableInterferogram(unsigned char(*temdata)[LENGTH], BLOCKINF block[], int& blocknum);
	void NoiseRemoveInterferogram(unsigned char(*image)[LENGTH]);
	void LablePt(unsigned char(*tempdata)[LENGTH],int x, int y,unsigned char bw,int num, int& TotalPts);
	bool Lable(unsigned char(*temdata)[LENGTH], unsigned char bw, int num, int& TotalPts);
	bool LableLeft(unsigned char(*temdata)[LENGTH], unsigned char bw,int num,int& TotalPts);//����������ȷ�������㷨
	bool LableRight(unsigned char(*temdata)[LENGTH], unsigned char bw,int num,int& TotalPts);//���ұ������ȷ�������㷨
	bool LableUp(unsigned char(*temdata)[LENGTH], unsigned char bw,int num,int& TotalPts);//���Ϸ������ȷ�������㷨
	bool LableDown(unsigned char(*temdata)[LENGTH], unsigned char bw,int num,int& TotalPts);//���·������ȷ�������㷨
	bool LableUpLeft(unsigned char(*temdata)[LENGTH], unsigned char bw,int num,int& TotalPts);//�����Ͻ������ȷ�������㷨
	bool LableUpRight(unsigned char(*temdata)[LENGTH], unsigned char bw,int num,int& TotalPts);//�����Ͻ������ȷ�������㷨
	bool LableDownLeft(unsigned char(*temdata)[LENGTH], unsigned char bw,int num,int& TotalPts);//�����½������ȷ�������㷨
	bool LableDownRight(unsigned char(*temdata)[LENGTH], unsigned char bw,int	num,int& TotalPts);//�����½������ȷ�������㷨
	void InitialBlockInf(BLOCKINF block[], int blockNum);//��ʼ������Ϣ
	void FindNeighbour(unsigned char(*image)[LENGTH],BLOCKINF block[],int number);//�������ڿ�
	void InsertNeighbourInf(BLOCKINF block[], int number, int Neigh);
	void DeleteUnwantedBlock(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum);//ɾ���Ǻ���Ŀ�				  
	bool IsMaxNeighbourNumGreaterThanTwo(BLOCKINF block[],int blockNum);//�ж�����ھ����Ƿ񳬹���������																
	void ReProcessBWMap(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum);//�������������2��������Զ�ֵ��ͼ����ʵ�������ϸ������						
	void PartialThin(unsigned char(*image)[LENGTH],BLOCKINF	block[],int num); //�Զ�ֵ��ͼ��Ĳ���������ϸ������																
	bool OrderInterferogram(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum);//����ͼ�ļ��δ���																		
	bool OrderFinished(BLOCKINF block[], int blockNum); //�жϼ���ȷ���Ƿ����										
	void DefineSignMap(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum,unsigned char(*sign)[LENGTH]);//����ͼ��ȷ��
	void DefineBlockExtremPoint(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum);//ȷ������ͼ��ÿ��ļ�ֵ��λ��
	bool FindUnlabedPt(unsigned char(*temdata)[LENGTH], int& x, int& y);
	void DefineThinFringe(unsigned char(*image)[LENGTH],BLOCKINF block[],int blockNum,unsigned char(*sign)[LENGTH],unsigned char(*fringe)[LENGTH]);//ȷ������ͼ������ϸ���Ƶ�λ��
	unsigned char GetSign(BLOCKINF block[], int num1, int num2);//��ȡ����																
	void CalculatePhase(unsigned char(*sign)[LENGTH], unsigned char(*fringe)[LENGTH]);//����λ��
	void cvShiftDFT(Mat src_arr);
	bool ContinueProcess(int& Px, int& Py);// Test if unwrapping should be continued
	bool ContRight(unsigned char flagNum);// continue in right direction
	bool ContDown(unsigned char flagNum);// continue in down direction
	bool ContLeft(unsigned char flagNum);// continue in left direction
	bool ContUp(unsigned char flagNum);// continue in Up direction
	bool Cont(); //�ӿ׾���̫��,�򷵻�FALSE,���򷵻�TRUE   // Continuning
	void Connect();
	void Ldlt(float a[], float b[], int n);	// Soving equation by ldlt	
	void FFT2D(int m, double *aa, double *bb);
	void FFT(int m, double *a, double *b);
	void IFFT(int m, double *a, double *b);
	void IFFT2D(int m, double *aa, double *bb);
	void Smooth();
	void ssmo1(int j1, int j2, int *a, int *b);
	void Sms();// Moving aligment error
	void sms_p();//moving tilt, piston
	void sms_s();// moving tilt, piston and focus
	void NoiseRemove();
	void  Unwrap(const int v1, int& v2);// Unwrapping
	void showtime(DWORD start, DWORD end);
	int		GetPTS();
	float	GetPV();
	float	GetRms();
	float	GetEms();
	float	GetN();
	float	GetDeltN(int flag);
	int		GetMinWavephase();
	int		GetMaxWavephase();
	void    GetContourCenter(std::vector< cv::Point>  contour, Point &p,int &radius);
	//LRESULT CinterferomeryDlg::StaticAnaysis(WPARAM wParam, LPARAM lParam);
	bool StaticAnaysis();
	float*		m_zernCoef;	// Zernike coefficients   
	float*      Sv;
	float		m_zern4[4]; // the first four coefficients
	float CalculateZern();
	CString DrawResult(Mat & frame);
	void Center(int *w, int size,float& center_x, float& center_y,float& r, int& istart, int& jstart, int& len);// Get center point and other parameters 
	void   GetCellName(int nRow, int nCol, CString &strName);
	afx_msg void OnManualset();
public:
	int m_aperType ;
	int m_aperCenterX ;
	int m_aperCenterY ;
	int m_aperRadius ;
	int m_aperHeight ;
	int m_aperWidth ;
	int m_aperRin ;
	int m_aperRout ;
	HBRUSH   m_hbrush;

	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//CStatusBarCtrl *m_StatBar;
	CListBox m_listBox;
	afx_msg void OnStart();
	afx_msg void OnAutomodel();
	afx_msg void OnNcPaint();
    void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    void OnDrawItem (int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	static BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam);
	CFont * f;
	NewMenu newMenu;
	void ReSize();
	int showimgsizex;
	int showimgsizey;
	POINT old;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnStaticAnalysis();
	afx_msg void OnSigle();
	afx_msg void OnMulty();
	afx_msg void OnPvPv();
	afx_msg void OnPvPvrms();
	afx_msg void OnPower();
	afx_msg void OnN();
	afx_msg void OnDeltan();
	afx_msg void Onaperture();
	afx_msg void OnAsia();
	afx_msg void OnPvFr();
	afx_msg void OnPvWave();
	afx_msg void OnPvum();
	afx_msg void OnPvNm();
	afx_msg void OnPvrmsFr();
	afx_msg void OnPvrmsWave();
	afx_msg void OnPvrmsum();
	afx_msg void OnPvrmsNm();
	afx_msg void OnPowerFr();
	afx_msg void OnPowerWave();
	afx_msg void OnPowerum();
	afx_msg void OnPowerNm();
	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void On32779();
	afx_msg void On32778();
	afx_msg void On32777();
	afx_msg void On32790();
	afx_msg void OnExcel();
	afx_msg void On32838();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAperset();
	afx_msg void On32843();
	afx_msg void On32844();
};
 