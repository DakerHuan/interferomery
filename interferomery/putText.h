#ifndef PUTTEXT_H_
#define PUTTEXT_H_

#include <windows.h>
#include <string>
//#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/utility.hpp>

using namespace cv;

void GetStringSize(HDC hDC, const char* str, int* w, int* h);
void putTextZH(Mat& dst, const char* str, Point org, Scalar color, int fontSize,	const char *fn = "Arial", bool italic = false, bool underline = false);

#endif // PUTTEXT_H_